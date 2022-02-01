#!/usr/bin/env ruby
#
# Democracy software compilation VERSION 0.0., created on 2021-11-14 17:41:33 -0500
# Get all of this code, and all of these files, with
# gem install democracy
# Linux ruby 2.7.0p0 (2019-12-25 revision 647ee6f091) [x86_64-linux-gnu]
#
# Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.





# 28 of 16 source files
module Gtk
require_relative "string__"
    class Window
    def self.new : self
      new type: Gtk::WindowType::TOPLEVEL
    end
  end

  class Builder
    def connect_signals
      connect_signals nil
    end

    def [](id)
      object(id).not_nil!
    end

    def []?(id)
      object(id)
    end
  end

  class CssProvider
    def load_from_data(string : String)
      load_from_data(string.to_slice)
    end
  end

  class Application
    def run
      argv = [PROGRAM_NAME]
      argv.concat(ARGV)
      run argv
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)