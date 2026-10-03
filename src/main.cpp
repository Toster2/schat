#include <algorithm>
#include <atomic>
#include <csignal>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

// ss -tlnp | grep 21370
// kill <pid>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#include "kissnet.hpp"
#include "tctl.hpp"

#define bytecast(a) reinterpret_cast<byte *>(&(a))

namespace kn = kissnet;
using namespace std;

constexpr int USERNAME = 16;
constexpr int MSG = 4096;

enum ClientMsgKind : uint8_t { Send, Login, Logout };

struct ClientMsg {
	ClientMsgKind kind;
	int len, namelen;
	char name[USERNAME];
	char msg[MSG];
};

enum ServerMsgKind : uint8_t { Msg };

struct ServerMsg {
	ServerMsgKind kind;
	int len;
	union {
		struct {
			char user[USERNAME];
			char msg[MSG];
		} msg;
		char joined[USERNAME];
	};
};

kn::socket_status recv_all(kn::tcp_socket &sock, byte *data, size_t size)
{
	size_t got = 0;
	while (got < size) {
		auto [n, ok] = sock.recv(data + got, size - got);
		if (ok != kn::socket_status::valid || n == 0)
			return ok;
		got += n;
	}
	return kn::socket_status::valid;
}

struct User {
	string name;
	int timeout;
};

struct ServerData {
	vector<string> msgs;
	vector<User> users;
};

struct Client {
	shared_ptr<kn::tcp_socket> sock;
	int id;
	Client(shared_ptr<kn::tcp_socket> s, int i) : sock(std::move(s)), id(i) {}
};

static mutex clients_mtx;
static vector<Client> clients;
static int next_id = 1;
static atomic<bool> g_running{true};
static kn::tcp_socket *g_listen_sock = nullptr;

static void on_signal(int)
{
	g_running = false;
	if (g_listen_sock)
		g_listen_sock->close();
}

void broadcast(const string &sender, const string &text, optional<int> exclude)
{
	lock_guard<mutex> lock(clients_mtx);
	for (size_t i = 0; i < clients.size();) {
		auto &c = clients[i];
		if (c.id == exclude) {
			++i;
			continue;
		}
		ServerMsg out = {};
		out.kind = Msg;
		out.len = (int)text.size();
		memcpy(out.msg.user, sender.data(), sender.size());
		memcpy(out.msg.msg, text.data(), text.size());
		auto [n, ok] = c.sock->send(bytecast(out), sizeof(out));
		if (ok != kn::socket_status::valid || n == 0) {
			cout << "klient " << c.id << " rozlaczony (send failure)\n";
			clients.erase(clients.begin() + i);
			continue;
		}
		++i;
	}
}

void handle_client(shared_ptr<kn::tcp_socket> sock, int id)
{
	string_view namev, msgv;
	ClientMsg msg = {};
	while (true) {
		auto ok = recv_all(*sock, bytecast(msg), sizeof(msg));
		if (ok != kn::socket_status::valid)
			break;

		namev = string_view(msg.name, msg.namelen);
		msgv = string_view(msg.msg, msg.len);

		switch (msg.kind) {
		case Send:
			cout << "user " << namev << " sent message '" << msgv << "'\n";
			broadcast(string(namev), string(msgv), nullopt);
			break;
		case Login:
			cout << namev << " logged in\n";
			broadcast("!", string(namev) + " dolaczyl", id);
			break;
		case Logout:
			cout << "logout wanted\n";
			break;
		}
	}

	{
		lock_guard<mutex> lock(clients_mtx);
		clients.erase(remove_if(clients.begin(), clients.end(), [id](const Client &c) { return c.id == id; }), clients.end());
	}
	cout << "klient " << id << " rozlaczony\n";
	broadcast("!", string(namev) + " uciekl", id);
}

void server(kn::port_t port)
{
	signal(SIGINT, on_signal);

	kn::tcp_socket listen_sock(kn::endpoint("0.0.0.0", port));
	g_listen_sock = &listen_sock;

	listen_sock.set_reuseaddr(true);
	listen_sock.bind();
	listen_sock.listen();

	g_listen_sock = &listen_sock;

	cout << "serwer slucha na porcie " << port << "\n";

	while (g_running) {
		auto cl = listen_sock.accept();
		if (!cl)
			continue;

		int id = next_id++;
		auto shared_sock = make_shared<kn::tcp_socket>(std::move(cl));
		{
			lock_guard<mutex> lock(clients_mtx);
			clients.emplace_back(shared_sock, id);
		}
		cout << "klient " << id << " polaczony (" << clients.size() << " online)\n";

		thread(handle_client, shared_sock, id).detach();
	}

	// Graceful shutdown: close all client sockets to unblock their recv calls
	{
		lock_guard<mutex> lock(clients_mtx);
		for (auto &c : clients) {
			c.sock->close();
		}
		clients.clear();
	}
	listen_sock.close();
	cout << "serwer zatrzymany\n";
}

void run(string name, string ip, kn::port_t port)
{
	tc::init();
	tc::enable_raw_mode();
	kn::tcp_socket sock;
	sock = kn::tcp_socket(kn::endpoint(ip, port));
	if (!sock.connect()) {
		cout << "cant connect\r\n";
		return;
	}
	{
		ClientMsg login = {};
		login.kind = Login;
		auto namelen = MIN(name.length(), USERNAME);
		memcpy(login.name, name.c_str(), namelen);
		login.namelen = namelen;
		sock.send(bytecast(login), sizeof(login));
	}
	cout << "!> polaczenie z serwerem " << ip << ":" << port << " zostalo zawarte\n";
	string s;
	ServerMsg m;
	while (true) {
		if (sock.select(kn::fds_read, 50) == kn::socket_status::valid && recv_all(sock, bytecast(m), sizeof(m)) == kn::socket_status::valid) {
			switch (m.kind) {
			case Msg:
				tc::clear_line();
				cout << m.msg.user << "> ";
				if (m.len > 0 && m.len < MSG)
					cout.write(m.msg.msg, m.len);
				cout << "\r\n";
				if (!s.empty())
					cout.write(s.data(), s.size());
				tc::flush();
			}
		}
		int c = tc::getch();
		if (c == -1)
			continue;
		switch (c) {
		case tc::KEY_BACKSPACE:
			if (!s.empty()) {
				s.pop_back();
				tc::cursor_left(1);
				tc::erase_chars(1);
			}
			break;
		case tc::KEY_ESC:
			return;
		case tc::KEY_RETURN:
			if (!s.empty()) {
				ClientMsg sm = {};
				sm.kind = Send;
				sm.len = (int)s.size();
				sm.namelen = name.length();
				memcpy(sm.name, name.c_str(), name.length());
				memcpy(sm.msg, s.data(), s.size());
				sock.send(bytecast(sm), sizeof(sm));
				tc::clear_line();
				s.erase();
			}
			break;
		case tc::KEY_LEFT:
		case tc::KEY_RIGHT:
		case tc::KEY_UP:
		case tc::KEY_DOWN:
			break;
		default:
			if (c > 0 && c < 256) {
				tc::write_char((char)c);
				s.push_back((char)c);
			}
			break;
		}
		tc::flush();
	}
	tc::disable_raw_mode();
	return;
}

int main(int argc, char **argv)
{
	kn::port_t port = 21370;
	if (argc != 2)
		cout << "bad args\n";
	switch (*argv[1]) {
	case 's':
		server(port);
		break;
	case 'c':
		string name, ip;
		cout << "username: ";
		getline(cin, name);
		cout << "server ip: ";
		getline(cin, ip);
		run(name, ip, port);
	}

	return 0;
}
