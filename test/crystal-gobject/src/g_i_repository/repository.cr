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





# 43 of 16 source files
module GIRepository
require_relative "string__"
 require_relative "./repository"
  class Repository
    @@instance : self?

    def self.instance
      @@instance ||= default
    end

    def require(namespace : String, version : String? = nil)
      self.require namespace, version, :zero_none
    end

    def dependencies(namespace : String)
      dependencies = previous_def
      dependencies.map { |dep|
        name, version = dep.split('-')
        {name, version}
      }
    end

    def find_by_name(namespace : String, name : String)
      ptr = LibGIRepository.repository_find_by_name(self, namespace, name)
      BaseInfo.wrap BaseInfo.new(ptr) if ptr
    end

    def all_infos(namespace : String)
      Array.new(n_infos(namespace)) { |index|
        info(namespace, index).not_nil!
      }
    end

    def info(namespace : String, index : Int)
      BaseInfo.wrap previous_def
    end

    def shared_library(namespace : String)
      libs = LibGIRepository.repository_get_shared_library(@pointer.as(LibGIRepository::Repository*), namespace)
      String.new(libs) if libs
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)