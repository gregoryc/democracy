#!/usr/bin/env ruby
# frozen_string_literal: true

puts File.read('gtk.html').gsub('\n', "\n").gsub(
  '\t', "\t"
)[1..-2].encode('ascii', invalid: :replace, undef: :replace, replace: '')
