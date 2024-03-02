#!/usr/bin/env ruby
#
# Democracy software compilation VERSION 0.0., created on 2021-11-14 17:41:34 -0500
# Get all of this code, and all of these files, with
# gem install democracy
# Linux ruby 2.7.0p0 (2019-12-25 revision 647ee6f091) [x86_64-linux-gnu]
#
# Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.





# 76 of 16 source files
module Assert
require_relative "string__"
    def assert(*args)
    raise if args.empty? && !self

    args.each { |i| raise [args, i].to_s unless i; }
  end; end

include Assert

class TrueClass
  include Assert
end

class FalseClass
  include Assert
end

class Object
  def in?(_array)
    false
  end

  def shellescape
    to_s.shellescape
  end
end

  if RUBY_ENGINE != 'opal'
	eval "require 'shellwords'"   #    ; end
end

#end


class String

  def puts
    Kernel.puts self
    self
  end

  def print
    Kernel.print self
    self
  end

  def in?(a)
    a.include? self
  end

  def ends_with?(a)
    end_with? a
  end

  def add_period
    if self[-1] == '.'
      self
    else
      "#{self}."; end
  end
end

class Array
  def which
    each do |cmd|
      exts = ENV['PATHEXT'] ? ENV['PATHEXT'].split(';') : ['']
      ENV['PATH'].split(File::PATH_SEPARATOR).each do |path|
        exts.each do |ext|
          exe = File.join(path, "#{cmd}#{ext}")
          return exe if File.executable?(exe) && !File.directory?(exe)
        end
      end
      nil
    end
  end
end

class String
  def uppercase
    return '' if self[0].nil?

    self[0].capitalize + slice(1, length)
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)