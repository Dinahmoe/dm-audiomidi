#include <iostream>
#include <stdexcept>
#include "MidiFile.hpp"


int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: dm_mididump midifile.mid" << std::endl;
  }
  
  try {
    dinahmoe::Smf::MidiFile mf(argv[1]);
    mf.dump();
  } catch (std::runtime_error ex) {
    std::cout << ex.what() << std::endl;
    return 1;
  }
  
  return 0;
}
