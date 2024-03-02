#!/usr/bin/env ruby
# frozen_string_literal: true

#
# Democracy software compilation VERSION 0.0., created on 2021-11-14 17:41:34 -0500
# Get all of this code, and all of these files, with
# gem install democracy
# Linux ruby 2.7.0p0 (2019-12-25 revision 647ee6f091) [x86_64-linux-gnu]
#
# Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

# 73 of 16 source files
def node?
`process`
true
rescue Exception
false
end

module Inputreceiver
  require_relative 'string__'



  class Inputreceiver
    def initialize(t = 'Enter text ')
      @title = @prompt = t
    end

    def to_s
      @text
    end

    def mode
      'zenity'
    end

    def speak_input
      return if RUBY_PLATFORM == 'opal'

      a = "echo #{@prompt} | #{voice = %w[flite espeak festival        cat].which}  "
      voice = File.basename voice
      b = { 'festival' => '--tts', 'flite' => '-voice slt', 'espeak' => '', 'cat' => '' }

      a += b[voice]
#puts a
      spawn a
    end

    def which(a)
      [a].which
    end

    def receive_input
      if ARGV[0].in? %w[mode modes]
        n = 0
        puts "Input modes\n".bold
        puts %w[browser-input stdin zenity dialog kdialog readline].sort.map {
               "  #{n += 1}. #{_1}"
             }.join("\n")
        puts
        exit
      end
      if ARGV.join.include? "s"
      speak_input
      end

      
      if browser_js? || (ARGV[0] == 'browser-js')
        get_input_from_browser
      elsif ARGV[0] != 'stdin' && ARGV[0] == 'kdialog' && which('kdialog')
        if RUBY_ENGINE != 'opal'
          `kdialog --inputbox #{@prompt.shellescape} --title #{@title.shellescape}`.strip
        end
      elsif ARGV[0] != 'stdin' && ARGV[0] == 'zenity' && which('zenity')
        if RUBY_ENGINE != 'opal'
          `zenity --entry --text #{@prompt.shellescape} --title #{@title.shellescape}`.strip
        end
      elsif ARGV[0] != 'stdin' && ARGV[0] == 'dialog' && which('dialog')
        eval 'require "tempfile"'
        text = ''
        Tempfile.open do |file|
          # puts "dialog --inputbox #{@title.shellescape} 0 0 2> #{file.to_path.shellescape}"
          system "dialog --inputbox #{@title.shellescape} 0 0 2> #{file.to_path.shellescape}"
          text = File.read(file)
        end
        text
      elsif (ARGV[0] == 'stdin') || (RUBY_ENGINE == 'opal')
        require 'opal/platform'
        txt = "#{@title} >>>  "
        print txt
        #        eval ("require opal/native")
        q = $stdin.readline.split("\n")

        q.first.to_s
      else
        tty = $stdin.tty?
        #      tty = true
        if tty
        eval 'require "readline"; require "colored"'
        txt = "#{@title} >>>  ".green if tty
        Readline.readline(txt).strip
        else
        STDIN.readline.strip
        end
      end
    end

    def get_input_from_browser
      require 'native'
      puts 'Democracy opal'
      $$.get_democracy_bar_input
    end

    def browser_js?
      `document`
      true
    rescue Exception
      false
    end


    def ruby?
      true
    end
  end
end

# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)
