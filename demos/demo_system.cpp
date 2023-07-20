#include <iostream>

#include "system.hpp"

int main() {

  std::cout << "utils::system::get_os() = " << utils::system::get_os()
            << std::endl;

  std::cout << "utils::system::get_num_cores() = "
            << utils::system::get_num_cores() << std::endl;

  std::cout << "utils::system::get_memory_GB() = "
            << utils::system::get_memory_GB() << "GB" << std::endl;

  std::cout << "utils::system::get_processor_name() = "
            << utils::system::get_processor_name() << std::endl;
}
