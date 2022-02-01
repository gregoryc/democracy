#!/usr/bin/python

import gtk
from sys import stdin

window = gtk.Window()

window.set_title("Shiltium Compiler Output")
window.set_default_size(613, 400)
window.move(100, 50)

text_view = gtk.TextView()
text_view.set_left_margin(10)
text_view.set_right_margin(10)
text_view.set_editable(False)
text_view.set_cursor_visible(False)
text_view.set_wrap_mode(gtk.WRAP_WORD)

buffer_ = text_view.get_buffer()
tag1 = buffer_.create_tag(pixels_above_lines = 10)
tag2 = buffer_.create_tag(font = "Monospace 10")

text = stdin.read()

lines = text.split("\n")
buffer_.insert_with_tags(buffer_.get_start_iter(), "\n".join(lines[1:]), tag2)
buffer_.insert_with_tags(buffer_.get_start_iter(), lines[0] + "\n", tag1)

scrolled_window = gtk.ScrolledWindow()
scrolled_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
scrolled_window.add(text_view)

window.add(scrolled_window)
window.connect("destroy", gtk.main_quit)

window.show_all()
gtk.main()
