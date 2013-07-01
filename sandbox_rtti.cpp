/*
  Copyright (c) 2013, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <type_traits>
#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include <sstream>
#include <fstream>
#include <iostream>

struct Base
{
  int y;
  virtual void foo() = 0;

  template<class Archive>
    void save(Archive & ar) const
    {
      std::cout << "Saving Base" << std::endl;
      ar( y );
    }

  template<class Archive>
    void load(Archive & ar)
    {
      std::cout << "Loading Base" << std::endl;
      ar( y );
    }
};

struct MyType : public Base
{
  int x;

  void foo() {}

  template<class Archive>
    void save(Archive & ar) const
    {
      std::cout << "Saving MyType" << std::endl;
      ar( cereal::virtual_base_class<Base>( this ) );
    }

  template<class Archive>
    void load(Archive & ar)
    {
      std::cout << "Loading MyType" << std::endl;
      ar( cereal::base_class<Base>( this ) );
    }
};
CEREAL_REGISTER_TYPE(MyType);

struct YourType : public Base
{
  YourType(int xx) : x(xx) {}
  YourType() : x(-1) {}
  int x;

  void foo() {}

  template<class Archive>
    void save(Archive & ar) const
    {
      std::cout << "Saving YourType" << std::endl;
      ar( x );
    }

  template<class Archive>
    void load(Archive & ar)
    {
      std::cout << "Loading YourType" << std::endl;
      ar( x );
    }
};
CEREAL_REGISTER_TYPE(YourType);

template <class T> void nop(T&&t) {}

int main()
{
  {
    std::ofstream ostream("rtti.txt");
    cereal::BinaryOutputArchive oarchive(ostream);

    std::shared_ptr<Base> ptr1 = std::make_shared<MyType>();
    std::shared_ptr<Base> ptr2 = std::make_shared<YourType>(33);
    std::unique_ptr<Base> ptr3(new MyType);
    std::weak_ptr<Base>   ptr4 = ptr2;

    oarchive(ptr1);
    oarchive(ptr2);
    oarchive(ptr3);
    oarchive(ptr4);
  }

  {
    std::ifstream istream("rtti.txt");
    cereal::BinaryInputArchive iarchive(istream);

    std::shared_ptr<Base> ptr1;
    std::shared_ptr<Base> ptr2;
    std::unique_ptr<Base> ptr3;
    std::weak_ptr<Base>   ptr4;

    iarchive(ptr1);
    iarchive(ptr2);
    iarchive(ptr3);
    iarchive(ptr4);
  }

  //std::remove("rtti.txt");
}
