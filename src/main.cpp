#include <iostream>
#include <type_traits>
#include <unistd.h> // for ftruncate
#include <memory>
#include <sys/mman.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <vector>
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


void HowToGetSharedPtrFromThis()
{
    class Person : public std::enable_shared_from_this<Person> {
      public:
        std::string name;
        Person(std::string n) : name(n) {}
        ~Person() {}

      public:
        void setParentsAndTheirKids(
            std::shared_ptr<Person> m = nullptr,
            std::shared_ptr<Person> f = nullptr) {
            mother = m;
            father = f;
            // this指针是一个普通指针，不是智能指针，所以不能直接赋值给shared_ptr
            //需要得到this指针的智能指针 可以使用enable_shared_from_this
            if (m != nullptr) { father->kids.push_back(shared_from_this()); }
            if (f != nullptr) { mother->kids.push_back(shared_from_this()); }
        }

      private:
        std::shared_ptr<Person> mother;
        std::shared_ptr<Person> father;
        std::vector<std::shared_ptr<Person>> kids;
    };
    shared_ptr<Person> mom(new Person("mom"));
    shared_ptr<Person> dad(new Person("dad"));
    shared_ptr<Person> kid(new Person("kid"));
    kid->setParentsAndTheirKids(mom, dad);
    cout << typeid(*kid).name() << endl;
}
int main() 
{

}