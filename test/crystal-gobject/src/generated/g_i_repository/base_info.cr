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





# 23 of 16 source files
module GIRepository
require_relative "string__"
    class BaseInfo
    include GObject::WrappedType

    def self.new : self
      new(Pointer(UInt8).malloc(72u64, 0u8).as(LibGIRepository::BaseInfo*))
    end
    GTYPE = LibGIRepository._g_base_info_gtype_get_type

    @pointer : Void*

    def initialize(pointer : LibGIRepository::BaseInfo*)
      @pointer = pointer.as(Void*)
    end

    def to_unsafe
      @pointer.not_nil!.as(LibGIRepository::BaseInfo*)
    end

    def to_unsafe_girepository_baseinfo
      @pointer.not_nil!.as(LibGIRepository::BaseInfo*)
    end

    def equal(info2 : GIRepository::BaseInfo)
      __var0 = LibGIRepository.base_info_equal(@pointer.as(LibGIRepository::BaseInfo*), info2.to_unsafe.as(LibGIRepository::BaseInfo*))
      (__var0 == 1)
    end

    def attribute(name : ::String)
      __var0 = LibGIRepository.base_info_get_attribute(@pointer.as(LibGIRepository::BaseInfo*), name.to_unsafe)
      GObject.raise_unexpected_null("g_base_info_get_attribute") if __var0.null?
      ::String.new(__var0)
    end

    def container
      __var0 = LibGIRepository.base_info_get_container(@pointer.as(LibGIRepository::BaseInfo*))
      GObject.raise_unexpected_null("g_base_info_get_container") if __var0.null?
      GIRepository::BaseInfo.new(__var0)
    end

    def name
      __var0 = LibGIRepository.base_info_get_name(@pointer.as(LibGIRepository::BaseInfo*))
      GObject.raise_unexpected_null("g_base_info_get_name") if __var0.null?
      ::String.new(__var0)
    end

    def namespace
      __var0 = LibGIRepository.base_info_get_namespace(@pointer.as(LibGIRepository::BaseInfo*))
      GObject.raise_unexpected_null("g_base_info_get_namespace") if __var0.null?
      ::String.new(__var0)
    end

    def type
      __var0 = LibGIRepository.base_info_get_type(@pointer.as(LibGIRepository::BaseInfo*))
      GIRepository::InfoType.new(__var0)
    end

    def typelib
      __var0 = LibGIRepository.base_info_get_typelib(@pointer.as(LibGIRepository::BaseInfo*))
      GObject.raise_unexpected_null("g_base_info_get_typelib") if __var0.null?
      GIRepository::Typelib.new(__var0)
    end

    def deprecated?
      __var0 = LibGIRepository.base_info_is_deprecated(@pointer.as(LibGIRepository::BaseInfo*))
      (__var0 == 1)
    end

    def iterate_attributes(iterator : GIRepository::AttributeIter)
      __var0 = LibGIRepository.base_info_iterate_attributes(@pointer.as(LibGIRepository::BaseInfo*), iterator, out name, out value)
      {(__var0 == 1), ::String.new(name), ::String.new(value)}
    end

    def dummy1
      to_unsafe.as(LibGIRepository::BaseInfo*).value.dummy1
    end

    def dummy2
      to_unsafe.as(LibGIRepository::BaseInfo*).value.dummy2
    end

    def dummy3
      to_unsafe.as(LibGIRepository::BaseInfo*).value.dummy3
    end

    def dummy4
      to_unsafe.as(LibGIRepository::BaseInfo*).value.dummy4
    end

    def dummy5
      to_unsafe.as(LibGIRepository::BaseInfo*).value.dummy5
    end

    def dummy6
      to_unsafe.as(LibGIRepository::BaseInfo*).value.dummy6
    end

    def dummy7
      to_unsafe.as(LibGIRepository::BaseInfo*).value.dummy7
    end

    def padding
      GObject::PointerIterator.new(to_unsafe.as(LibGIRepository::BaseInfo*).value.padding) { |__var0|
        __var0
      }
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)