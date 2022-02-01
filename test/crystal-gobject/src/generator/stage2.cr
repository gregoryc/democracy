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





# 3 of 16 source files
module GLib
require_relative "string__"
 require_relative ".//"
require_relative "./repository"
  class Error < Exception
    def self.assert(error)
      raise new(error) if error
    end

    def initialize(gerror : LibGLib::GError*)
      super String.new(gerror.value.message)
    end
  end
end

macro each_converted(prefix, basename, type, plural = nil)
  def each_{{basename.id}}
    n = LibGIRepository.{{prefix.id}}_get_n_{{plural ? plural.id : "#{basename}s".id}}(self)
    0.upto(n-1) do |index|
      yield {{type.id}}.new LibGIRepository.{{prefix.id}}_get_{{basename.id}}(self, index)
    end
  end
end


module GIRepository
  class BaseInfo
    def initialize(@ptr)
    end

    def to_unsafe
      @ptr
    end
  end

  class Repository
    include GObject::WrappedType

    def initialize(@ptr)
    end

    def to_unsafe
      @ptr
    end

    def require(namespace, version, flags)
      __error = Pointer(LibGLib::Error).null
      __return_value = LibGIRepository.repository_require((to_unsafe as LibGIRepository::Repository*), namespace, version, flags, pointerof(__error))
      GLib::Error.assert __error
    end

    def dependencies(namespace)
      __return_value = LibGIRepository.repository_get_dependencies((to_unsafe as LibGIRepository::Repository*), namespace)
      GObject::PointerIterator.new(__return_value) { |__item_76| raise "Expected string but got null" unless __item_76; String.new(__item_76) }
    end

    def shared_library(namespace)
      __return_value = LibGIRepository.repository_get_shared_library((to_unsafe as LibGIRepository::Repository*), namespace)
      raise "Expected string but got null" unless __return_value; String.new(__return_value)
    end

    def n_infos(namespace)
      __return_value = LibGIRepository.repository_get_n_infos((to_unsafe as LibGIRepository::Repository*), namespace)
      __return_value
    end

    def info(namespace, index)
      __return_value = LibGIRepository.repository_get_info((to_unsafe as LibGIRepository::Repository*), namespace, Int32.new(index))
      GIRepository::BaseInfo.new(__return_value)
    end

    def c_prefix(namespace)
      __return_value = LibGIRepository.repository_get_c_prefix((to_unsafe as LibGIRepository::Repository*), namespace)
      raise "Expected string but got null" unless __return_value; String.new(__return_value)
    end

    def self.default
      new LibGIRepository.repository_get_default
    end
  end

  def self.filename(filename)
    filename.gsub(/[A-Z][a-z]*(?=[A-Z])/) { |m| "#{m.downcase}_" }
      .gsub(/::/, "_")
      .downcase
  end
end









# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)