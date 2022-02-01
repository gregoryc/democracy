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





# 29 of 16 source files
module GLib
require_relative "string__"
    enum Priority
    Default      = LibGLib::PRIORITY_DEFAULT
    Default_Idle = LibGLib::PRIORITY_DEFAULT_IDLE
    High         = LibGLib::PRIORITY_HIGH
    High_Idle    = LibGLib::PRIORITY_HIGH_IDLE
    Low          = LibGLib::PRIORITY_LOW
  end

  def self.idle_add(priority : Priority = Priority::Default, &block : -> Bool)
    trampoline = LibGLib::SourceFunc.new { block.call ? 1 : 0 }
    LibGLib.idle_add(
      priority,
      LibGLib::SourceFunc.new(trampoline.pointer, Pointer(Void).null),
      GObject::ClosureDataManager.register(trampoline.closure_data),
      ->GObject::ClosureDataManager.deregister
    )
  end

  def self.timeout_seconds(interval : UInt32, priority : Priority = Priority::Default, &block : -> Bool)
    raise ArgumentError.new("Timeout must be at least 1 second") unless interval > 0
    trampoline = LibGLib::SourceFunc.new { block.call ? 1 : 0 }
    sourcefunc = LibGLib::SourceFunc.new(trampoline.pointer, Pointer(Void).null)
    closure = GObject::ClosureDataManager.register(trampoline.closure_data)
    LibGLib.timeout_add_seconds(
      priority,
      interval,
      sourcefunc,
      closure,
      ->GObject::ClosureDataManager.deregister
    )
  end

  def self.timeout_milliseconds(interval : UInt32, priority : Priority = Priority::Default, &block : -> Bool)
    raise ArgumentError.new("Timeout must be at least 1 millisecond") unless interval > 0
    trampoline = LibGLib::SourceFunc.new { block.call ? 1 : 0 }
    sourcefunc = LibGLib::SourceFunc.new(trampoline.pointer, Pointer(Void).null)
    closure = GObject::ClosureDataManager.register(trampoline.closure_data)
    LibGLib.timeout_add(
      priority,
      interval,
      sourcefunc,
      closure,
      ->GObject::ClosureDataManager.deregister
    )
  end

  def self.timeout(interval : Time::Span, priority : Priority = Priority::Default, &block : -> Bool)
    if interval.to_i == interval.total_seconds || interval.total_milliseconds > UInt32::MAX
      self.timeout_seconds(interval.total_seconds.to_u32, priority, &block)
    else
      self.timeout_milliseconds(interval.total_milliseconds.to_u32, priority, &block)
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)