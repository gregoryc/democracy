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





# 37 of 16 source files
module GObject
require_relative "string__"
    VERSION = "0.10.0"
  LOADED_NAMESPACES = [] of String

  def self.raise_unexpected_null(symbol : String)
    raise <<-EOF
      An unexpected null pointer was returned from #{symbol}.

      This means that there is a missing may return null annotation in the GIR
      file defining this function. Please report this as a bug upstream.

      For now please manually override the generated wrapper method with one that
      handles the null pointer. You may also want to upstream this override to the
      shard maintaining manual overrides for the respective library.
      EOF
  end
end

macro require_gobject(namespace, version = nil)
  {% unless GObject::LOADED_NAMESPACES.includes?(namespace) %}
    {% GObject::LOADED_NAMESPACES << namespace %}
    {% if version %}
      {{run "gobject/generator/build_namespace", namespace, version}}
    {% else %}
      {{run "gobject/generator/build_namespace", namespace}}
    {% end %}
  {% end %}
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)