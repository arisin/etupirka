#include "etupirka.hxx"

int main(const int number_of_arguments, const char* const* const arguments)
{
  using namespace arisin::etupirka;
  logger::initialize();
  const auto mode = commandline_helper::interpret({arguments, arguments + number_of_arguments});
  etupirka_t etupirka(mode);
  etupirka.run();
}