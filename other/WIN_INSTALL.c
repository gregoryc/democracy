#include <stdio.h>
#include <windows.h>

static void ruby_dialog() {
  MessageBox(0,
             "You need Ruby (14MB).\n\n\n\nRuby is a powerful programming "
             "language. It ships by default on Macs.\n\n\n\nProceeding to "
             "download the installer... ",
             "Ruby needs to be installed", 0);
}

int main() {
  while (system("ruby -v && gem -v") != 0) {
    MessageBox(0,
               "You need Ruby (14MB). Ruby is a powerful programming language. "
               "It ships by default on Macs. Installing now... ",
               "Ruby needs to be installed", 0);
    system("start /b iexplore.exe "
           "https://github.com/oneclick/rubyinstaller2/releases/download/"
           "RubyInstaller-3.0.2-1/rubyinstaller-3.0.2-1-x64.exe");
  }
  MessageBox(0, "Ruby and Gem detected. Installing Democracy", "Ruby", 0);
  system("gem install democracy");
}
