# frozen_string_literal: true

Gem::Specification.new do |s|
  s.name        = 'democracy'
  s.version     = "0.0.#{File.read('VERSION_NUMBER').strip.to_i}"
  s.summary     = 'Trying to help with democracy'
  s.description = "github.com/gregoryc/democracy #{File.read("README").split.join(" ")[0..230]}"
  s.authors     = ['Gregory Cohen']
  s.email       = 'gregorycohen2@gmail.com'
  s.files       = `find -type f | fgrep -v nohup.out`.split("\n")
  s.homepage    = 'https://www.ethicify.online'
  s.license = '0BSD'
  s.executables = Dir.glob('../bin/*')
end
