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





# 46 of 16 source files
module GIRepository
require_relative "string__"
    class Dumper
    macro def
      def dump(dumper, & : BaseInfo ->)
        super(dumper) {|i| yield i }
        {{yield}}
      end
    end

    macro dump_childs(kind, plural = nil)
      {% plural = plural ? plural : "#{kind}s".id %}
      if {{plural}}_size > 0
        dumper.puts "* {{plural}}"
        dumper.nest(2) do
          each_{{kind}} {|c| yield c }
        end
      end
    end

    macro dump_child(name)
      %child = {{name}}
      if %child
        dumper.puts "* {{name}}"
        dumper.nest(2) do
          yield %child
        end
      end
    end

    getter io

    @nesting = [] of BaseInfo
    @indent = 0
    @max_nesting = 3

    def initialize(@io : IO)
    end

    def initialize(@io : IO, @max_nesting : Int32?)
    end

    def puts(str)
      @io << indent
      @io.puts str
    end

    def nest(inc = 1)
      @indent += inc
      yield
      @indent -= inc
    end

    def base_indent
      "  " * (@indent - 1)
    end

    def indent
      "  " * @indent
    end

    def nesting_limit?
      max_nesting = @max_nesting
      return false unless max_nesting
      @nesting.size >= max_nesting
    end

    def dump(info)
      @nesting << info
      nest do
        info.dump(self) do |child|
          next unless child

          if @nesting.includes?(child)
            child.dump_base(self)
            puts "<...> (self referential)"
            next
          end

          if nesting_limit? || child.info_type.unresolved?
            child.dump_base(self)
            next
          end

          dump(child)
        end
      end
      @nesting.pop
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)