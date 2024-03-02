# frozen_string_literal: true

Gem::Specification.new do |s|
  s.name        = 'democracy'
  s.version     = "1.0.#{File.read('VERSION_NUMBER').strip.to_i}"
  s.summary     = 'Trying to help with democracy'
  s.description = "#{File.read("README")}"
  s.authors     = ['Gregory Cohen']
  s.email       = 'gregorycohen2@gmail.com'
  s.files       = `find -type f | fgrep -v nohup.out`.split("\n")
  s.homepage    = 'https://www.github.com/gregoryc/democracy'
  s.license = '0BSD'
  s.executables = Dir.glob('../bin/*')
end
