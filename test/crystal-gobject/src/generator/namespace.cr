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





# 4 of 16 source files
      module_functions_definition(builder)
require_relative "string__"
 require_relative "./repository"

      each_info do |info|
        info.wrapper_definition(builder, libname)
      end
    end
  end

  def write_wrapper(directory, name)
    prefix = File.join(directory, GIRepository.filename(@namespace))
    Dir.mkdir_p prefix

    info = GIRepository::Repository.instance.find_by_name(@namespace, name)
    if info
      wrapper_path = File.join(directory, GIRepository.filename(@namespace), "#{GIRepository.filename(info.name)}.cr")
      write_wrapper(wrapper_path, info) { |builder| info.wrapper_definition(builder, libname) }
    else
      STDERR.puts "Warning: Couldn't find #{@namespace}/#{name}"
    end
  end

  def write_wrappers(directory)
    prefix = File.join(directory, GIRepository.filename(@namespace))
    Dir.mkdir_p prefix

    wrapper_path = File.join(prefix, "#{GIRepository.filename(@namespace)}.cr")
    File.open(wrapper_path, "w") do |io|
      io.puts %(require "../../g_object/wrapped_type")
      io.puts %(require "../#{lib_filename}")
      io.puts
      GIRepository::Repository.instance.dependencies(@namespace).each do |dependency|
        namespace, version = dependency
        name = Namespace.new(namespace).wrapper_filename
        path = "../../#{name}" if File.exists? File.join(directory, "..", name) # Overrides
        path ||= "../#{name}" if File.exists? File.join(directory, name)        # Wrapper
        io.puts %(require "#{path}") if path
      end
      io.puts
      io.puts %(require "./*")
    end

    each_info do |info|
      wrapper_path = File.join(directory, GIRepository.filename(@namespace), "#{GIRepository.filename(info.name)}.cr")
      write_wrapper(wrapper_path, info) { |builder| info.wrapper_definition(builder, libname) }
    end

    write_module_functions_wrapper(directory)
  end

  def write_module_functions_wrapper(directory)
    prefix = File.join(directory, GIRepository.filename(@namespace))
    Dir.mkdir_p prefix
    write_wrapper(File.join(prefix, "module_functions.cr")) { |builder| module_functions_definition(builder) }
  end

  private def each_info
    infos = GIRepository::Repository.instance.all_infos(@namespace).select { |info|
      next false if skip_info? info
      next false if info.is_a? GIRepository::FunctionInfo
      next false if info.is_a? GIRepository::ConstantInfo
      true
    }

    sort_childs_after_parents(infos)

    infos.each do |info|
      yield info
    end
  end

  private def sort_childs_after_parents(infos)
    names = infos.map { |info|
      info.full_constant if info.is_a?(GIRepository::ObjectInfo) || info.is_a?(GIRepository::InterfaceInfo)
    }

    parents = infos.map { |info|
      parent_names = [] of String
      if info.is_a?(GIRepository::ObjectInfo)
        parent = info.parent
        parent_names << parent.full_constant if parent
        parent_names.concat info.interfaces.map(&.full_constant)
      end

      parent_names
    }

    child_index = 0
    while child_index < names.size
      next child_index += 1 if names[child_index].nil?

      swapped = false
      child_index.upto(names.size - 1) do |index|
        if parents[child_index].includes? names[index]
          swap(infos, child_index, index)
          swap(names, child_index, index)
          swap(parents, child_index, index)
          swapped = true
          break
        end
      end

      child_index += 1 unless swapped
    end
  end

  private def swap(items, a, b)
    tmp = items[a]
    items[a] = items[b]
    items[b] = tmp
  end

  private def module_functions_definition(builder)
    builder.section do
      GIRepository::Repository.instance.all_infos(@namespace).select { |info|
        info.is_a?(GIRepository::ConstantInfo)
      }.each(&.wrapper_definition(builder, libname))
    end

    GIRepository::Repository.instance.all_infos(@namespace).select { |info|
      next false if skip_info?(info)
      info.is_a?(GIRepository::FunctionInfo) ? !info.method? : false
    }.each(&.wrapper_definition(builder, libname))
  end

  private def write_wrapper(path, info = nil)
    File.open(path, "w") do |io|
      Crout.build(io) do |builder|
        if info && info.is_a?(GIRepository::ObjectInfo)
          parent = info.parent
          if parent && parent.namespace == @namespace
            io.puts %(require "./#{GIRepository.filename(parent.name)}")
            io.puts
          end
        end

        wrapper_definition(builder) { |builder| yield builder }
      end
    end
  end

  private def wrapper_definition(builder, source_path = nil)
    if source_path
      builder.source_location(source_path) do
        builder.def_module(Namespace.constant(@namespace)) { yield builder }
      end
    else
      builder.def_module(Namespace.constant(@namespace)) { yield builder }
    end
  end

  def self.constant(string)
    "#{string[0].upcase}#{string[1..-1]}"
  end

  private def skip_info?(info)
    case info
    when GIRepository::EnumInfo
      name = info.name
      return true unless name
      return true unless 'A' <= name[0] <= 'Z' # Bug in typelib?
    else
      false
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)