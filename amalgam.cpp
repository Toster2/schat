#define N INVALID_SOCKET
#define K addr
#define Y char
#define D const
#define J constexpr
#define M getaddrinfo_results
#define I if
#define L inline
#define P int
#define C kissnet_fatal_error
#define O multicastRequest
#define U non_blocking_would_have_blocked
#define X nullptr
#define S protocol
#define G received_bytes
#define E reinterpret_cast
#define A return
#define Z sizeof
#define F sock
#define T sock_proto
#define H socket_addrinfo
#define Q socket_input_socklen
#define B socket_status
#define W static_cast
#define V struct
#define R void
#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <conio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define AI_ADDRCONFIG 0x00000400
#ifndef SHUT_RDWR
#define SHUT_RDWR SD_BOTH
#endif
#define KISSNET_OS_SPECIFIC_PAYLOAD_NAME wsa_ptr
#define KISSNET_OS_SPECIFIC std::shared_ptr<kissnet::win32_specific::WSA> KISSNET_OS_SPECIFIC_PAYLOAD_NAME
#define KISSNET_OS_INIT KISSNET_OS_SPECIFIC_PAYLOAD_NAME = kissnet::win32_specific::getWSA()
using ioctl_setting=u_long;using buffsize_t=P;L D Y*inet_ntop(P af,D R*src,Y*dst,socklen_t size){union{V sockaddr sa;V sockaddr_in sai;V sockaddr_in6 sai6;}K;P res;memset(&K,0,Z(K));K.sa.sa_family=(unsigned short)af;I(af==AF_INET6){memcpy(&K.sai6.sin6_addr,src,Z(K.sai6.sin6_addr));}else{memcpy(&K.sai.sin_addr,src,Z(K.sai.sin_addr));}res=WSAAddressToStringA(&K.sa,Z(K),0,dst,E<LPDWORD>(&size));I(res!=0)A NULL;A dst;}
#pragma comment(lib, "Ws2_32.lib")
namespace kissnet{namespace win32_specific{V WSA;namespace internal_state{static WSA*global_WSA=X;}V WSA:std::enable_shared_from_this<WSA>{WSA(D WSA&)=delete;WSA&operator=(D WSA&)=delete;WSA(WSA&&)=delete;WSA&operator=(WSA&&)=delete;WSADATA wsa_data;WSA():wsa_data{}{I(D auto status=WSAStartup(MAKEWORD(2,2),&wsa_data);status!=0){std::string error_message;switch(status){default:error_message="Unknown error happened.";break;case WSASYSNOTREADY:error_message="The underlying network subsystem is not ready ""for network communication.";break;case WSAVERNOTSUPPORTED:error_message=" The version of Windows Sockets support requested ""(2.2)"" is not provided by this particular Windows Sockets ""implementation. ";break;case WSAEINPROGRESS:error_message="A blocking Windows Sockets 1.1 operation is in progress.";break;case WSAEPROCLIM:error_message="A limit on the number of tasks supported by the Windows ""Sockets implementation has been reached.";break;case WSAEFAULT:error_message="The lpWSAData parameter is not a valid pointer.";break;}C(error_message);}}~WSA(){WSACleanup();internal_state::global_WSA=X;}std::shared_ptr<WSA>getPtr(){A shared_from_this();}};L std::shared_ptr<WSA>getWSA(){I(internal_state::global_WSA)A internal_state::global_WSA->getPtr();auto wsa=std::make_shared<WSA>();internal_state::global_WSA=wsa.get();A wsa;}}L P get_error_code(){D auto error=WSAGetLastError();switch(error){case WSAEWOULDBLOCK:A EWOULDBLOCK;case WSAEBADF:A EBADF;case WSAEINTR:A EINTR;default:A error;}}}
#else // UNIX platform
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#define KISSNET_OS_SPECIFIC_PAYLOAD_NAME dummy
#define KISSNET_OS_SPECIFIC char dummy
#define KISSNET_OS_INIT dummy = 42;
using ioctl_setting=P;using buffsize_t=size_t;static D P N=-1;static D P SOCKET_ERROR=-1;using SOCKET=P;using SOCKADDR_IN=sockaddr_in;using SOCKADDR=sockaddr;using IN_ADDR=in_addr;L P closesocket(SOCKET in){A close(in);}template<typename...Params>L P ioctlsocket(P fd,P request,Params&&...params){A ioctl(fd,request,params...);}namespace unix_specific{}L P get_error_code(){A errno;}
#endif // ifdef WIN32
#ifndef SOL_TCP
#define SOL_TCP IPPROTO_TCP
#endif
#define MIN(x,y)((x)<(y)?(x):(y))
#define bytecast(a) reinterpret_cast<byte *>(&(a))
namespace tc{L R init(R){
#ifdef _WIN32
HANDLE h_out=GetStdHandle(STD_OUTPUT_HANDLE);I(h_out!=INVALID_HANDLE_VALUE){DWORD mode=0;I(GetConsoleMode(h_out,&mode))SetConsoleMode(h_out,mode|ENABLE_VIRTUAL_TERMINAL_PROCESSING);}
#endif
}L R flush(R){fflush(stdout);}L R write(D Y*s){fputs(s,stdout);}L R write_char(Y c){fputc(c,stdout);}L R seq(D Y*s){fputs(s,stdout);}L R seqn(D Y*prefix,P n,Y suffix){Y buf[32];snprintf(buf,Z(buf),"%s%d%c",prefix,n,suffix);fputs(buf,stdout);}L R cursor_left(P n){seqn("\x1b[",n,'D');}L R cursor_right(P n){seqn("\x1b[",n,'C');}L R cursor_home(R){seq("\r");}L R cursor_line_end(R){seqn("\x1b[",999,'C');}L R cursor_to_col(P col){seqn("\x1b[",col,'G');}L R clear_line(R){seq("\x1b[2K\r");}L R clear_to_end(R){seq("\x1b[K");}L R clear_to_start(R){seq("\x1b[1K");}L R erase_chars(P n){I(n>0)seqn("\x1b[",n,'P');}J P KEY_ESC=0x1b;J P KEY_RETURN=0x0d;J P KEY_BACKSPACE=0x7f;J P KEY_UP=1000;J P KEY_DOWN=1001;J P KEY_LEFT=1002;J P KEY_RIGHT=1003;
#ifdef _WIN32
L P getch_raw(R){I(!_kbhit())A-1;A _getch();}L P enable_raw_mode(R){A 0;}L R disable_raw_mode(R){}
#else
static P g_raw=0;static V termios g_orig;L P getch_raw(R){unsigned Y c;I(read(STDIN_FILENO,&c,1)!=1)A-1;A c;}L P read_byte_timeout(unsigned Y*c,long usec){fd_set set;V timeval tv={0,usec};P r;FD_ZERO(&set);FD_SET(STDIN_FILENO,&set);I(select(STDIN_FILENO+1,&set,NULL,NULL,&tv)>0){r=getch_raw();I(r>=0){*c=(unsigned Y)r;A 1;}}A 0;}L P enable_raw_mode(R){V termios raw;extern R disable_raw_mode(R);I(g_raw)A 0;I(tcgetattr(STDIN_FILENO,&g_orig)==-1)A-1;raw=g_orig;raw.c_iflag&=~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);raw.c_cflag|=CS8;raw.c_lflag&=~(ECHO|ICANON|IEXTEN);raw.c_cc[VMIN]=0;raw.c_cc[VTIME]=0;I(tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw)==-1)A-1;g_raw=1;atexit(disable_raw_mode);A 0;}L R disable_raw_mode(R){I(g_raw){tcsetattr(STDIN_FILENO,TCSAFLUSH,&g_orig);g_raw=0;}}
#endif /* _WIN32 */
L P getch(R){P c=getch_raw();I(c==-1)A-1;
#ifdef _WIN32
I(c==0||c==0xe0){switch(getch_raw()){case 72:A KEY_UP;case 80:A KEY_DOWN;case 75:A KEY_LEFT;case 77:A KEY_RIGHT;default:A KEY_ESC;}}I(c==8)A KEY_BACKSPACE;I(c==KEY_RETURN)A KEY_RETURN;A c;
#else
I(c==0x1b){unsigned Y seq[2];I(read_byte_timeout(&seq[0],50000)&&(seq[0]=='['||seq[0]=='O')&&read_byte_timeout(&seq[1],50000)){switch(seq[1]){case'A':A KEY_UP;case'B':A KEY_DOWN;case'C':A KEY_RIGHT;case'D':A KEY_LEFT;default:A KEY_ESC;}}A KEY_ESC;}I(c==0x08)A KEY_BACKSPACE;I(c==KEY_RETURN)A KEY_RETURN;A c;
#endif
}}
#define kissnet_fatal_error(STR) throw std::runtime_error(STR)
namespace kissnet{namespace error{static R(*callback)(D std::string&,R*ctx)=X;static R*ctx=X;static bool abortOnFatalError=true;L R handle(D std::string&str){I(callback){callback(str,ctx);}else{fputs(str.c_str(),stderr);}I(abortOnFatalError){abort();}}}enum class S{tcp,tcp_ssl,udp};V addr_collection{sockaddr_storage adrinf={0};socklen_t sock_size=0;};static J P fds_read=0x1;static J P fds_write=0x2;static J P fds_except=0x4;template<size_t buff_size>using buffer=std::array<std::byte,buff_size>;using port_t=uint16_t;V endpoint{std::string address{};port_t port{};endpoint()=default;endpoint(std::string K,port_t prt):address{std::move(K)},port{prt}{}static bool is_valid_port_number(unsigned long n){A n<1<<16;}endpoint(std::string K){D auto separator=K.find_last_of(':');I(separator==std::string::npos)C("string is not of address:port form");I(separator==K.size()-1)C("string has ':' as last character. Expected port number here");address=K.substr(0,separator);D auto parsed_port=strtoul(K.substr(separator+1).c_str(),X,10);I(!is_valid_port_number(parsed_port))C("Invalid port number "+std::to_string(parsed_port));port=W<port_t>(parsed_port);}endpoint(SOCKADDR*K){switch(K->sa_family){case AF_INET:{auto ip_addr=(SOCKADDR_IN*)(K);address=inet_ntoa(ip_addr->sin_addr);port=ntohs(ip_addr->sin_port);}break;case AF_INET6:{auto ip_addr=(sockaddr_in6*)(K);Y buffer[INET6_ADDRSTRLEN];address=inet_ntop(AF_INET6,&(ip_addr->sin6_addr),buffer,INET6_ADDRSTRLEN);port=ntohs(ip_addr->sin6_port);}break;default:{C("Trying to construct an endpoint for a protocol familly that ""is neither AF_INET or AF_INET6");}}I(address.empty())C("Couldn't construct endpoint from sockaddr(_storage) struct");}};L auto syscall_socket=[](P af,P type,P S){A::socket(af,type,S);};L auto syscall_select=[](P nfds,fd_set*readfds,fd_set*writefds,fd_set*exceptfds,V timeval*timeout){A::select(nfds,readfds,writefds,exceptfds,timeout);};L auto syscall_recv=[](SOCKET s,Y*buff,buffsize_t len,P flags){A::recv(s,buff,len,flags);};L auto syscall_send=[](SOCKET s,D Y*buff,buffsize_t len,P flags){A::send(s,buff,len,flags);};L auto syscall_bind=[](SOCKET s,D V sockaddr*name,socklen_t namelen){A::bind(s,name,namelen);};L auto syscall_connect=[](SOCKET s,D V sockaddr*name,socklen_t namelen){A::connect(s,name,namelen);};L auto syscall_listen=[](SOCKET s,P backlog){A::listen(s,backlog);};L auto syscall_accept=[](SOCKET s,V sockaddr*K,socklen_t*addrlen){A::accept(s,K,addrlen);};L auto syscall_shutdown=[](SOCKET s){A::shutdown(s,SHUT_RDWR);};V B{enum values:int8_t{errored=0x0,valid=0x1,cleanly_disconnected=0x2,U=0x3,timed_out=0x4};D values value;B():value{errored}{}explicit B(bool state):value(values(state?valid:errored)){}B(values v):value(v){}B(D B&)=default;B(B&&)=default;operator bool()D{A value>0;}int8_t get_value()D{A value;}bool operator==(values v)D{A v==value;}bool operator!=(values v)D{A v!=value;}};template<S T>class socket{using bytes_with_status=std::tuple<size_t,B>;KISSNET_OS_SPECIFIC;SOCKET F=N;endpoint bind_loc={};addrinfo getaddrinfo_hints={};addrinfo*M=X;addrinfo*H=X;R initialize_addrinfo(){P type{};P iprotocol{};I J(T==S::tcp||T==S::tcp_ssl){type=SOCK_STREAM;iprotocol=IPPROTO_TCP;}else I J(T==S::udp){type=SOCK_DGRAM;iprotocol=IPPROTO_UDP;}getaddrinfo_hints={};getaddrinfo_hints.ai_family=AF_UNSPEC;getaddrinfo_hints.ai_socktype=type;getaddrinfo_hints.ai_protocol=iprotocol;getaddrinfo_hints.ai_flags=AI_ADDRCONFIG;}B connect(addrinfo*K,int64_t timeout,bool createsocket){I J(T==S::tcp||T==S::tcp_ssl){I(createsocket){close();H=X;F=syscall_socket(K->ai_family,K->ai_socktype,K->ai_protocol);}I(F==N)A B::errored;H=K;I(timeout>0)set_non_blocking(true);P error=syscall_connect(F,K->ai_addr,socklen_t(K->ai_addrlen));I(error==SOCKET_ERROR){error=get_error_code();I(error==EWOULDBLOCK||error==EAGAIN||error==EINPROGRESS){V timeval tv;tv.tv_sec=W<long>(timeout/1000);tv.tv_usec=1000*W<long>(timeout%1000);fd_set fd_write,fd_except;;FD_ZERO(&fd_write);FD_SET(F,&fd_write);FD_ZERO(&fd_except);FD_SET(F,&fd_except);P ret=syscall_select(W<P>(F)+1,NULL,&fd_write,&fd_except,&tv);I(ret==-1)error=get_error_code();else I(ret==0)error=ETIMEDOUT;else{socklen_t errlen=Z(error);I(getsockopt(F,SOL_SOCKET,SO_ERROR,E<Y*>(&error),&errlen)!=0)C("getting socket error returned an error");}}}I(timeout>0)set_non_blocking(false);I(error==0){A B::valid;}else{close();H=X;A B::errored;}}else{C("connect called for non-tcp socket");}}sockaddr_storage socket_input={};socklen_t Q=0;public:socket()=default;socket(D socket&)=delete;socket&operator=(D socket&)=delete;socket(socket&&other)noexcept{KISSNET_OS_SPECIFIC_PAYLOAD_NAME=std::move(other.KISSNET_OS_SPECIFIC_PAYLOAD_NAME);bind_loc=std::move(other.bind_loc);F=std::move(other.F);socket_input=std::move(other.socket_input);Q=std::move(other.Q);M=std::move(other.M);H=std::move(other.H);other.F=N;other.M=X;other.H=X;}socket&operator=(socket&&other)noexcept{I(this!=&other){I(!(F<0)||F!=N)closesocket(F);KISSNET_OS_SPECIFIC_PAYLOAD_NAME=std::move(other.KISSNET_OS_SPECIFIC_PAYLOAD_NAME);bind_loc=std::move(other.bind_loc);F=std::move(other.F);socket_input=std::move(other.socket_input);Q=std::move(other.Q);M=std::move(other.M);H=std::move(other.H);other.F=N;other.M=X;other.H=X;}A*this;}bool operator==(D socket&other)D{A F==other.F;}bool is_valid()D{A F!=N;}L operator bool()D{A is_valid();}socket(endpoint bind_to):bind_loc{std::move(bind_to)}{KISSNET_OS_INIT;initialize_addrinfo();D std::string K=bind_loc.address;I(K=="localhost"||K=="localhost.localdomain"||K=="localhost6"||K=="localhost6.localdomain6"||K=="127.0.0.1"||K=="::1"){getaddrinfo_hints.ai_flags=0;}I(getaddrinfo(K.c_str(),std::to_string(bind_loc.port).c_str(),&getaddrinfo_hints,&M)!=0){C("getaddrinfo failed!");}for(auto*K=M;K;K=K->ai_next){F=syscall_socket(K->ai_family,K->ai_socktype,K->ai_protocol);I(F!=N){H=K;break;}}I(F==N){C("unable to create socket!");}}socket(SOCKET native_sock,endpoint bind_to):F{native_sock},bind_loc(std::move(bind_to)){KISSNET_OS_INIT;initialize_addrinfo();}R set_non_blocking(bool state=true)D{
#ifdef _WIN32
ioctl_setting set=state?1:0;I(ioctlsocket(F,FIONBIO,&set)<0)
#else
D auto flags=fcntl(F,F_GETFL,0);D auto newflags=state?(flags|O_NONBLOCK):(flags&~O_NONBLOCK);I(fcntl(F,F_SETFL,newflags)<0)
#endif
C("setting socket to nonblock returned an error");}R set_reuseaddr(bool state=false)D{D P reuse=state?1:0;I(setsockopt(F,SOL_SOCKET,SO_REUSEADDR,E<D Y*>(&reuse),Z(reuse))!=0)C("setting socket broadcast mode returned an error");}R set_broadcast(bool state=true)D{D P broadcast=state?1:0;I(setsockopt(F,SOL_SOCKET,SO_BROADCAST,E<D Y*>(&broadcast),Z(broadcast))!=0)C("setting socket broadcast mode returned an error");}R set_tcp_no_delay(bool state=true)D{I J(T==S::tcp){D P tcpnodelay=state?1:0;I(setsockopt(F,SOL_TCP,TCP_NODELAY,E<D Y*>(&tcpnodelay),Z(tcpnodelay))!=0)C("setting socket tcpnodelay mode returned an error");}}B get_status()D{P sockerror=0;socklen_t errlen=Z(sockerror);I(getsockopt(F,SOL_SOCKET,SO_ERROR,E<Y*>(&sockerror),&errlen)!=0)C("getting socket error returned an error");A sockerror==SOCKET_ERROR?B::errored:B::valid;}R bind(){I(syscall_bind(F,W<SOCKADDR*>(H->ai_addr),socklen_t(H->ai_addrlen))==SOCKET_ERROR){C("bind() failed\n");}}R join(D endpoint&multi_cast_endpoint,D std::string&interface=""){I(T!=S::udp){C("joining a multicast is only possible in UDP mode\n");}addrinfo*multicast_addr;addrinfo*local_addr;addrinfo hints={0};hints.ai_family=PF_UNSPEC;hints.ai_flags=AI_NUMERICHOST;I(getaddrinfo(multi_cast_endpoint.address.c_str(),X,&hints,&multicast_addr)!=0){C("getaddrinfo() failed\n");}hints.ai_family=multicast_addr->ai_family;hints.ai_socktype=SOCK_DGRAM;hints.ai_flags=AI_PASSIVE;I(getaddrinfo(X,std::to_string(multi_cast_endpoint.port).c_str(),&hints,&local_addr)!=0){C("getaddrinfo() failed\n");}F=syscall_socket(local_addr->ai_family,local_addr->ai_socktype,local_addr->ai_protocol);I(F!=N){H=local_addr;}else{C("syscall_socket() failed\n");}bind();I(multicast_addr->ai_family==PF_INET&&multicast_addr->ai_addrlen==Z(V sockaddr_in)){V ip_mreq O={0};memcpy(&O.imr_multiaddr,&((V sockaddr_in*)(multicast_addr->ai_addr))->sin_addr,Z(O.imr_multiaddr));I(interface.length()){O.imr_interface.s_addr=inet_addr(interface.c_str());;}else{O.imr_interface.s_addr=htonl(INADDR_ANY);}I(setsockopt(F,IPPROTO_IP,IP_ADD_MEMBERSHIP,(Y*)&O,Z(O))!=0){C("setsockopt() failed\n");}}else I(multicast_addr->ai_family==PF_INET6&&multicast_addr->ai_addrlen==Z(V sockaddr_in6)){V ipv6_mreq O={0};memcpy(&O.ipv6mr_multiaddr,&((V sockaddr_in6*)(multicast_addr->ai_addr))->sin6_addr,Z(O.ipv6mr_multiaddr));I(interface.length()){V addrinfo*reslocal;I(getaddrinfo(interface.c_str(),X,X,&reslocal)){C("getaddrinfo() failed\n");}O.ipv6mr_interface=((sockaddr_in6*)reslocal->ai_addr)->sin6_scope_id;freeaddrinfo(reslocal);}else{O.ipv6mr_interface=0;}I(setsockopt(F,IPPROTO_IPV6,IPV6_JOIN_GROUP,(Y*)&O,Z(O))!=0){C("setsockopt() failed\n");}}else{C("unknown AI family.\n");}freeaddrinfo(multicast_addr);}B connect(int64_t timeout=0){I J(T==S::tcp){auto curr_addr=H;I(connect(curr_addr,timeout,false)!=B::valid){for(auto*K=M;K;K=K->ai_next){I(K==curr_addr)continue;I(connect(K,timeout,true)==B::valid)break;}}I(F==N)C("unable to create connectable socket!");A B::valid;}}R listen(){I J(T==S::tcp){I(syscall_listen(F,SOMAXCONN)==SOCKET_ERROR){C("listen failed\n");}}}socket accept(){I J(T!=S::tcp){A{N,{}};}sockaddr_storage socket_address;SOCKET s;socklen_t size=Z socket_address;I((s=syscall_accept(F,E<SOCKADDR*>(&socket_address),&size))==N){D auto error=get_error_code();switch(error){case EWOULDBLOCK:case EINTR:A{};}C("accept() returned an invalid socket\n");}A{s,endpoint(E<SOCKADDR*>(&socket_address))};}R close(){I(F!=N){closesocket(F);}F=N;}R shutdown(){I(F!=N){syscall_shutdown(F);}}~socket(){close();I(M)freeaddrinfo(M);}B select(P fds,int64_t timeout){fd_set fd_read,fd_write,fd_except;;V timeval tv;tv.tv_sec=W<long>(timeout/1000);tv.tv_usec=1000*W<long>(timeout%1000);I(fds&fds_read){FD_ZERO(&fd_read);FD_SET(F,&fd_read);}I(fds&fds_write){FD_ZERO(&fd_write);FD_SET(F,&fd_write);}I(fds&fds_except){FD_ZERO(&fd_except);FD_SET(F,&fd_except);}P ret=syscall_select(W<P>(F)+1,fds&fds_read?&fd_read:NULL,fds&fds_write?&fd_write:NULL,fds&fds_except?&fd_except:NULL,&tv);I(ret==-1)A B::errored;else I(ret==0)A B::timed_out;A B::valid;}template<size_t buff_size>bytes_with_status send(D buffer<buff_size>&buff,D size_t length=buff_size,addr_collection*K=X){assert(buff_size>=length);A send(buff.data(),length,K);}bytes_with_status send(D std::byte*read_buff,size_t length,addr_collection*K=X){auto G{0};I J(T==S::tcp){G=syscall_send(F,E<D Y*>(read_buff),W<buffsize_t>(length),0);}else I J(T==S::udp){I(K){G=sendto(F,E<D Y*>(read_buff),W<buffsize_t>(length),0,E<sockaddr*>(&K->adrinf),K->sock_size);}else{G=sendto(F,E<D Y*>(read_buff),W<buffsize_t>(length),0,W<SOCKADDR*>(H->ai_addr),socklen_t(H->ai_addrlen));}}I(G<0){I(get_error_code()==EWOULDBLOCK){A{0,B::U};}A{0,B::errored};}A{G,B::valid};}template<size_t buff_size>bytes_with_status recv(buffer<buff_size>&write_buff,size_t start_offset=0,addr_collection*addr_info=X){auto G=0;I J(T==S::tcp){G=syscall_recv(F,E<Y*>(write_buff.data())+start_offset,W<buffsize_t>(buff_size-start_offset),0);}else I J(T==S::udp){Q=Z socket_input;G=::recvfrom(F,E<Y*>(write_buff.data())+start_offset,W<buffsize_t>(buff_size-start_offset),0,E<sockaddr*>(&socket_input),&Q);I(addr_info){addr_info->adrinf=socket_input;addr_info->sock_size=Q;}}I(G<0){D auto error=get_error_code();I(error==EWOULDBLOCK)A{0,B::U};I(error==EAGAIN)A{0,B::U};A{0,B::errored};}I(G==0){A{G,B::cleanly_disconnected};}A{size_t(G),B::valid};}bytes_with_status recv(std::byte*buffer,size_t len,bool wait=true,addr_collection*addr_info=X){auto G=0;I J(T==S::tcp){P flags;I(wait)flags=MSG_WAITALL;else{
#ifdef _WIN32
flags=0;set_non_blocking(true);
#else
flags=MSG_DONTWAIT;
#endif
}G=syscall_recv(F,E<Y*>(buffer),W<buffsize_t>(len),flags);
#ifdef _WIN32
set_non_blocking(false);
#endif
}else I J(T==S::udp){Q=Z socket_input;G=::recvfrom(F,E<Y*>(buffer),W<buffsize_t>(len),0,E<sockaddr*>(&socket_input),&Q);I(addr_info){addr_info->adrinf=socket_input;addr_info->sock_size=Q;}}I(G<0){D auto error=get_error_code();I(error==EWOULDBLOCK)A{0,B::U};I(error==EAGAIN)A{0,B::U};A{0,B::errored};}I(G==0){A{G,B::cleanly_disconnected};}A{size_t(G),B::valid};}endpoint get_bind_loc()D{A bind_loc;}endpoint get_recv_endpoint()D{I J(T==S::tcp){A get_bind_loc();}I J(T==S::udp){D SOCKADDR*K=E<D SOCKADDR*>(&socket_input);A endpoint(const_cast<SOCKADDR*>(K));}}size_t bytes_available()D{static ioctl_setting size=0;D auto status=ioctlsocket(F,FIONREAD,&size);I(status<0){C("ioctlsocket status is negative when getting FIONREAD\n");}A size>0?size:0;}static S get_protocol(){A T;}};using tcp_socket=socket<S::tcp>;using udp_socket=socket<S::udp>;}namespace kn=kissnet;using namespace std;J P USERNAME=16;J P MSG=4096;enum ClientMsgKind:uint8_t{Send,Login,Logout};V ClientMsg{ClientMsgKind kind;P len,namelen;Y name[USERNAME];Y msg[MSG];};enum ServerMsgKind:uint8_t{Msg};V ServerMsg{ServerMsgKind kind;P len;union{V{Y user[USERNAME];Y msg[MSG];}msg;Y joined[USERNAME];};};kn::B recv_all(kn::tcp_socket&F,byte*data,size_t size){size_t got=0;while(got<size){auto[n,ok]=F.recv(data+got,size-got);I(ok!=kn::B::valid||n==0)A ok;got+=n;}A kn::B::valid;}V User{string name;P timeout;};V ServerData{vector<string>msgs;vector<User>users;};V Client{shared_ptr<kn::tcp_socket>F;P id;Client(shared_ptr<kn::tcp_socket>s,P i):F(std::move(s)),id(i){}};static mutex clients_mtx;static vector<Client>clients;static P next_id=1;static atomic<bool>g_running{true};static kn::tcp_socket*g_listen_sock=X;static R on_signal(P){g_running=false;I(g_listen_sock)g_listen_sock->close();}R broadcast(D string&sender,D string&text,optional<P>exclude){lock_guard<mutex>lock(clients_mtx);for(size_t i=0;i<clients.size();){auto&c=clients[i];I(c.id==exclude){++i;continue;}ServerMsg out={};out.kind=Msg;out.len=(P)text.size();memcpy(out.msg.user,sender.data(),sender.size());memcpy(out.msg.msg,text.data(),text.size());auto[n,ok]=c.F->send(bytecast(out),Z(out));I(ok!=kn::B::valid||n==0){cout<<"klient "<<c.id<<" rozlaczony (send failure)\n";clients.erase(clients.begin()+i);continue;}++i;}}R handle_client(shared_ptr<kn::tcp_socket>F,P id){string_view namev,msgv;ClientMsg msg={};while(true){auto ok=recv_all(*F,bytecast(msg),Z(msg));I(ok!=kn::B::valid)break;namev=string_view(msg.name,msg.namelen);msgv=string_view(msg.msg,msg.len);switch(msg.kind){case Send:cout<<"user "<<namev<<" sent message '"<<msgv<<"'\n";broadcast(string(namev),string(msgv),nullopt);break;case Login:cout<<namev<<" logged in\n";broadcast("!",string(namev)+" dolaczyl",id);break;case Logout:cout<<"logout wanted\n";break;}}{lock_guard<mutex>lock(clients_mtx);clients.erase(remove_if(clients.begin(),clients.end(),[id](D Client&c){A c.id==id;}),clients.end());}cout<<"klient "<<id<<" rozlaczony\n";broadcast("!",string(namev)+" uciekl",id);}R server(kn::port_t port){signal(SIGINT,on_signal);kn::tcp_socket listen_sock(kn::endpoint("0.0.0.0",port));g_listen_sock=&listen_sock;listen_sock.set_reuseaddr(true);listen_sock.bind();listen_sock.listen();g_listen_sock=&listen_sock;cout<<"serwer slucha na porcie "<<port<<"\n";while(g_running){auto cl=listen_sock.accept();I(!cl)continue;P id=next_id++;auto shared_sock=make_shared<kn::tcp_socket>(std::move(cl));{lock_guard<mutex>lock(clients_mtx);clients.emplace_back(shared_sock,id);}cout<<"klient "<<id<<" polaczony ("<<clients.size()<<" online)\n";thread(handle_client,shared_sock,id).detach();}{lock_guard<mutex>lock(clients_mtx);for(auto&c:clients){c.F->close();}clients.clear();}listen_sock.close();cout<<"serwer zatrzymany\n";}R run(string name,string ip,kn::port_t port){name=name.substr(0,USERNAME);tc::init();tc::enable_raw_mode();kn::tcp_socket F;F=kn::tcp_socket(kn::endpoint(ip,port));I(!F.connect()){cout<<"cant connect\r\n";A;}{ClientMsg login={};login.kind=Login;memcpy(login.name,name.c_str(),name.length());login.namelen=name.length();F.send(bytecast(login),Z(login));}cout<<"!> polaczenie z serwerem "<<ip<<":"<<port<<" zostalo zawarte\n";string s;ServerMsg m;while(true){I(F.select(kn::fds_read,50)==kn::B::valid&&recv_all(F,bytecast(m),Z(m))==kn::B::valid){switch(m.kind){case Msg:tc::clear_line();cout<<m.msg.user<<"> ";I(m.len>0&&m.len<MSG)cout.write(m.msg.msg,m.len);cout<<"\r\n";I(!s.empty())cout.write(s.data(),s.size());tc::flush();}}P c=tc::getch();I(c==-1)continue;switch(c){case tc::KEY_BACKSPACE:I(!s.empty()){s.pop_back();tc::cursor_left(1);tc::erase_chars(1);}break;case tc::KEY_ESC:A;case tc::KEY_RETURN:I(!s.empty()){ClientMsg sm={};sm.kind=Send;sm.len=(P)s.size();sm.namelen=name.length();s=s.substr(0,MSG);memcpy(sm.name,name.c_str(),name.length());memcpy(sm.msg,s.data(),s.size());F.send(bytecast(sm),Z(sm));tc::clear_line();s.erase();}break;case tc::KEY_LEFT:case tc::KEY_RIGHT:case tc::KEY_UP:case tc::KEY_DOWN:break;default:I(c>0&&c<256){tc::write_char((Y)c);s.push_back((Y)c);}break;}tc::flush();}tc::disable_raw_mode();A;}P main(P argc,Y**argv){kn::port_t port=21370;I(argc!=2)cout<<"bad args\n";switch(*argv[1]){case's':server(port);break;case'c':string name,ip;cout<<"username: ";getline(cin,name);cout<<"server ip: ";getline(cin,ip);run(name,ip,port);}A 0;}