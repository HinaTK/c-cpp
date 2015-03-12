/**
 * @file CompatSocket.h
 * @brief ������Socket��װ
 *        ����Windows Socket��BSD Socket
 *        ���ṩ����socket����(������)
 * Licensed under the MIT licenses.
 *
 * @note
 * �����ͻ��˵���int connect(int sockfd, const struct sockaddr *addr, socklen_t len);����Է�������socket��������������ͻ���socket������Ϊ����ģʽ���һֱ���������ӽ�����������ʧ��(ע������ģʽ�ĳ�ʱʱ�����Ϊ75�뵽������֮��)�������Ϊ������ģʽ�������connect֮��������Ӳ������Ͻ����򷵻�-1(errno����ΪEINPROGRESS��ע������Ҳ�������Ͻ����ɹ��������ӱ����ķ���������),���û�����Ͻ������أ���ʱTCP����·���ֶ����ڱ������������������������Ķ�����Ȼ�����select��������connect�Ƿ����(��ʱ����ָ��select�ĳ�ʱʱ�䣬�����ʱʱ���������Ϊ��connect�ĳ�ʱʱ���)�����select��ʱ��ر�socket��Ȼ����Գ��Դ����µ�socket�������ӣ����select���ط�����socket��������д��������ӽ����ɹ������select���ط�����socket�������ȿɶ��ֿ�д��������ӳ���(ע�⣺������������һ������������������ֿ������������ӽ�������֮�󣬷������˷��������ݸ��ͻ��ˣ���ʱselectͬ���᷵�ط�����socket�������ȿɶ��ֿ�д����ʱ����ͨ�����·�������:
 *��1.����getpeername��ȡ�Զ˵�socket��ַ.���getpeername����ENOTCONN,��ʾ���ӽ���ʧ��,Ȼ����SO_ERROR����getsockopt�õ��׽ӿ��������ϵĴ��������;
 *��2.����read,��ȡ����Ϊ0�ֽڵ�����.���read����ʧ��,���ʾ���ӽ���ʧ��,����read���ص�errnoָ��������ʧ�ܵ�ԭ��.������ӽ����ɹ�,readӦ�÷���0;
 *��3.�ٵ���һ��connect.��Ӧ��ʧ��,�������errno��EISCONN,�ͱ�ʾ�׽ӿ��Ѿ�����,���ҵ�һ�������ǳɹ���;����,���Ӿ���ʧ�ܵ�;
 *    ���������ӵ�socket����(SOCK_DGRAM)���ͻ���Ҳ���Ե���connect��������,������ʵ���ϲ�����������SOCK_STREAM�����ӣ����������ڱ��ر����˶Զ˵ĵ�ַ�����������Ķ�д��������Ĭ�������ӵĶԶ�Ϊ��������
 *
 * @version 1.0
 * @author OWenT
 * @date 2013.04.26
 *
 * @history
 *
 */

#pragma once

#include <string>
#include <list>

#ifdef WIN32
    #include <WinSock2.h>
    #include <ws2tcpip.h>
    typedef int                socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/tcp.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <errno.h>
    #include <unistd.h>
    typedef int                SOCKET;

    //#pragma region define win32 const variable in linux
    #define INVALID_SOCKET    -1
    #define SOCKET_ERROR    -1
    //#pragma endregion
#endif

namespace util
{

    namespace socket
    {

        struct DnsInfo {
            struct ADDR_TYPE {
                enum type {
                    UNIX = AF_UNIX,
                    IPV4 = AF_INET,
                    IPV6 = AF_INET6,
                };
            };
            
            ADDR_TYPE::type type;
            std::string address;
        };

        class CompatSocket 
        {
        public:
            typedef std::list<DnsInfo> dns_result_t;

        public:
            CompatSocket(SOCKET sock = INVALID_SOCKET);
            ~CompatSocket();

            // set option
            int SetOption(int optmame, const void* optval, int optlen, int level = SOL_SOCKET);

            // Create socket object for snd/recv data
            bool Create(int af = PF_INET, int type = SOCK_STREAM, int protocol = 0);

            // Connect socket
            bool Connect(const char* ip, uint16_t port, DnsInfo::ADDR_TYPE::type type = DnsInfo::ADDR_TYPE::IPV4);
            // #region server
            // Bind socket
            bool Bind(uint16_t port);

            // Listen socket
            bool Listen(int backlog = 5); 

            // Accept socket
            bool Accept(CompatSocket& s, DnsInfo* from);
            // #endregion
            
            bool GetPeerName(DnsInfo& peer);

            // Send socket
            int Send(const char* buf, int len);

            // Recv socket
            int Recv(char* buf, int len);
            
            // Select
            int Select(bool read, bool write, int iSecond = 0, int iMicroSeconds = 0);

            // Close socket
            int Close();

            // Get errno
            int GetError();
            
            //#pragma region just for win32
            // Init winsock DLL 
            static int Init();    
            // Clean winsock DLL
            static int Clean();
            //#pragma endregion

            // Domain parse
            static bool DnsParse(const char* domain, std::list<DnsInfo>& dns_res);

            CompatSocket& operator = (SOCKET s);

            operator SOCKET ();

            bool IsValid() const;

            void SetNoBlock(bool no_block = false);

            void SetNoDelay(bool no_delay = false);

            void SetKeepAlive(bool keep_alive = false);

            int SetTimeout(int recvtimeout, int sendtimeout, int lingertimeout);
        protected:
            SOCKET m_uSock;

        private:
            template<typename TSrc, typename TD>
            void _assign(TSrc& s, const TD& td) {
                s = static_cast<TSrc>(td);
            }
        };

    }
}
