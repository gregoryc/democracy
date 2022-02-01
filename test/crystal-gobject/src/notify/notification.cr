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





# 33 of 16 source files
module Notify
require_relative "string__"
    class Notification
    class Fields
      property! summary : String
      property! body : String
      property! icon_name : String
      property! app_name : String
      property! timeout : Int32
      property! category : String
      property! urgency : Symbol | Notify::Urgency
      getter actions

      def initialize
        @transient = false
        @resident = false
        @actions = [] of {String, String, ActionCallback}
      end

      def transient?
        @transient
      end

      def transient
        @transient = true
      end

      def resident?
        @resident
      end

      def resident
        @resident
      end

      def action(name, label, &block : ActionCallback)
        @actions << {name, label, block}
      end
    end

    alias ActionCallback = String ->

    def self.build
      fields = Fields.new

      with fields yield fields

      new(fields.summary, fields.body?, fields.icon_name?).tap do |notification|
        notification.app_name = fields.app_name if fields.app_name?
        notification.timeout = fields.timeout if fields.timeout?
        notification.category = fields.category if fields.category?
        notification.urgency = fields.urgency if fields.urgency?
        notification.transient if fields.transient?
        notification.resident if fields.resident?
        fields.actions.each do |action|
          name, label, block = action
          notification.action name, label, &block
        end
      end
    end

    def action(name, label, &block : ActionCallback)
      LibNotify.notification_add_action(
        self,
        name,
        label,
        ->Notification.handle_action,
        GObject::ClosureDataManager.register(Box.box(block)),
        ->GObject::ClosureDataManager.deregister
      )
    end

    def self.handle_action(notification, action, callback)
      action = String.new(action)
      Box(ActionCallback).unbox(callback).call action
    end

    def set_hint(name, value)
      case value
      when Bool
        set_hint_uint32 name, value ? 1_u32 : 0_u32
      when String
        set_hint_string name, value
      when Int
        set_hint_int32 name, value
      end
    end

    def clear_hints
      LibNotify.notification_clear_hints self
    end

    def transient
      set_hint "transient", true
    end

    def resident
      set_hint "resident", true
    end

    def urgency=(urgency)
      if urgency.is_a?(Symbol)
        urgency = case urgency
                  when :low
                    Notify::Urgency::LOW
                  when :normal
                    Notify::Urgency::NORMAL
                  when :critical
                    Notify::Urgency::CRITICAL
                  else
                    raise ArgumentError.new "#{urgency} is not a valid urgency level"
                  end
      end

      self.urgency = urgency
    end

    def update(summary = nil, body = nil, icon = nil)
      summary ||= self.summary.not_nil!
      body ||= self.body
      icon ||= self.icon_name
      body = body ? body.to_unsafe : Pointer(UInt8).null
      icon = icon ? icon.to_unsafe : Pointer(UInt8).null

      LibNotify.notification_update self, summary, body, icon

      show
    end

    private def get(property)
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)