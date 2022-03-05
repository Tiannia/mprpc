#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;


int main()
{
    LoginResponse loginrsp; //登录响应
    ResultCode *rc = loginrsp.mutable_result();//mutable是可变的意思
    rc->set_errcode(1);
    rc->set_errmsg("登录处理失败了");
    
    std::cout << rc->SerializeAsString() << std::endl;

    GetFriendListsResponse rsp;
    rc = rsp.mutable_result();
    rc->set_errcode(0);

    User *user1 = rsp.add_friend_list();
    user1->set_name("zhang san");
    user1->set_age(20);
    user1->set_sex(User::MAN);

    User *user2 = rsp.add_friend_list();
    user2->set_name("li si");
    user2->set_age(22);
    user2->set_sex(User::MAN);

    std::cout << rsp.friend_list_size() << std::endl;

    return 0;
}
