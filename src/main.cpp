#include <iostream>
#include <type_traits>
#include <unistd.h> // for ftruncate
#include <memory>
#include <sys/mman.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
using namespace std;

shared_ptr<int> getIntSharedMemory(int num)
{ 
    void* mem;
    int shmfd = shm_open("/temp1234", O_CREAT| O_RDWR ,0770);
    if (shmfd < 0) {
        throw std::string(strerror(errno));
    }
    if (ftruncate(shmfd, sizeof(int)* num) < 0) {
        throw std::string(strerror(errno));
    }
    mem = mmap(nullptr,num * sizeof(int) , PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if ( mem == MAP_FAILED)
    {
        throw std::string(strerror(errno));
    }
    return {static_cast<int*>(mem),[=](int* p){
            if(shm_unlink("/temp1234")!=0)
            {
                std::cerr<<"unlink failed"<<std::endl;
            }
            close(shmfd);
    }};
}
void TestMem()
{
   shared_ptr<int> ism(getIntSharedMemory(100));
    for(int i=0;i<100;i++)
    {
        ism.get()[i]=i;
    }
    cout<<"return"<<endl;
    cout<<ism.use_count()<<endl;

    ism.reset();
}
void TestSmartPointerWrongWayToUse()
{
    //use_count为1 会多次释放同一对象 导致程序崩溃 要共享应该是智能指针赋值
    int *p = new int;
    shared_ptr<int> sp1(p);
    shared_ptr<int> sp2(p);
    cout<<sp1.use_count()<<endl;
    cout<<sp2.use_count()<<endl;
}
int main() {

}