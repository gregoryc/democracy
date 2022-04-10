#!/usr/bin/ruby
# frozen_string_literal: true

require 'fileutils'
require 'shellwords'
require 'colored'

class ColorCycle
  # require 'colored'
  @@f = ''.colors.dup
  unless $stdout.tty?
    def @@f.pop
      'to_s'
    end
  end
  def self.print(text)
    a = @@f.pop
    text = text.to_s
    t = nil
    if a
    else
      @@f = ''.colors.dup
      a = @@f.pop
    end
    t = text.send a
    t = "\n\n#{t}\n\n"
    Kernel.print t
  end
end

def c_code(a, prepend = false)
  $c_code = '' if $c_code.nil?
  if prepend
    $c_code = "#{a}#{$c_code}"
  else
    $c_code += a
  end
end

def add_c_header
  c_code <<~EOF
    //text=
        #define c g_signal_connect
         int main(int argc, char* argv[]) {
         	#ifndef linux
         		_putenv("GTK_CSD=0");

         	#endif
         	gtk_init(&argc, &argv);
         	GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

         	gtk_window_set_title(
       window,#{'  	                    '}
         	"The top section has to be run in the command prompt. The bottom section has to be run in the browser console on Indeed. First run bottom, then top. "
         );
         	GtkWidget* inspect_widget_0 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    //     	gtk_window_set_size_request(400, 400)
    		{ const int size = 400;
         	gtk_widget_set_size_request(window, size,   size); }
  EOF
end

def add_html_footer
  html '</html>'
end

def add_html_header
  html '<!DOCTYPE html>'
end

def add_c_footer
  c_code <<EOF
	    	c(window, "destroy", gtk_main_quit, NULL);
	    	    	gtk_container_add(GTK_CONTAINER(window),                   inspect_widget_0);
    	gtk_widget_set_size_request(window, 500, 500);
    	gtk_widget_show_all(GTK_WIDGET(window));
    	gtk_main();
    //	gtk_widget_unref(window);
    }

EOF
end
$file = File.open 'html_file', 'w'
def html(text)
  $file.print text
end

def quit
  $file.close

  sites = [
    "https://www.indeed.com/",

    'https://mail.google.com/mail/?view=cm&fs=1&to=&su=Job+Application&body=Sincerely,+Gregory+Cohen'
  ]

  sites = sites.map(&:shellescape)

  sites = sites.join ' '

  # sites = sites.join(" ")

  c_code($lists.flatten.map do |i|
           "static long int #{i}(GtkTextView* t)

{
    GtkTextIter start, end;
   GtkTextBuffer  *buf = gtk_text_view_get_buffer(t);gtk_text_buffer_get_start_iter(buf,&start);

    gtk_text_buffer_get_end_iter(buf, &end);
    gtk_text_buffer_select_range(buf, &start, &end);

    GtkClipboard *clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);(gtk_text_buffer_copy_clipboard(buf,clip));\n\tfputs(#{i.dump}, stderr);  chdir(\"/Program Files/Google/Chrome/Application\"); system(#{"google-chrome                  #{sites} ".dump}); system(#{"start chrome.exe   #{sites}   ".dump}); return 0     ;\n}\n\n"
         end.uniq.join("\n").to_s, 1)
  'gtk/gtk stdlib stdio'.split do |lib|
    c_code "#include <#{lib}.h>\n", true
  end

  gtk = 'gtk+-3.0'
  File.write 'c_program.c', $c_code
  #	Gtk.main_qit

  #	Gtk::main_quit
  f = '/usr/lib/mxe/usr/bin/x86_64-w64-mingw32.static'
  a = [
    `#{f}-pkg-config --libs --cflags #{gtk} | tee /dev/stderr`.strip, "#{f}-gcc c_program.c -o c_program.exe -O3 -s"
  ]
  a[1] += " #{a[0]}"

  a[1] += ' && clear && '
  a[1] += 'wine c_program.exe'
  spawn a.last
a="gcc -O3 -s c_program.c -o linux_c_program #{`pkg-config --cflags --libs #{gtk}`.strip}  ;clear; ./linux_c_program"
spawn a

end





class FalseClass
  def to_i
    0
  end

  def +(other)
    to_i other
  end
end

class TrueClass
  def to_i
    1
  end

  def +(other)
    to_i other
  end
end

class Array
  @@item = 0
  def inspect(recurring = false)
    #  @	@@item += 1
    begin
      begin
        require 'vte'
      rescue Exception
        system "i='libgirepository-dev ruby-gtk3-dev'
 sudo apt install $i || sudo yum install $i
 sudo gem install vte"
        system "ruby #{__FILE__} #{ARGV.join}"
        exit
      end
      box = if recurring
              Gtk::HBox.new
            else
              Gtk::VBox.new
            end
    rescue Exception => e
      html 'Rescue'
      require 'gtk3'
      box = Gtk::Box.new(recurring ? :horizontal : :vertical)
    end
    box.set_size_request(30, 30)
    item_number = @@item
    unless recurring
      add_c_header
      add_html_header
    end
    i = map do # { |i|
      i = _1
      if i.instance_of? Array
        i
      else
        [i]
      end
    end
    if map { |i| i.instance_of? Array }.sum(&:to_i).zero?
      each do |item|
        html '<span>'
        e = Gtk::TextView.new
        b = Gtk::TextBuffer.new
        e.set_buffer(b)
        e.show
        text = item.to_s
        html text
        #  @@ite
        @@item += 1
        add_c_code_for_packing text, item_number
        b.text = text
        html '</span>'
        e.set_size_request(30, 30)
        box.pack_start(e)
        begin
          vte = Vte::Terminal.new
          vte.fork_command(working_directory: Dir.home)
          box.pack_start(vte)
        rescue StandardError => e
        end
      end
    else
      before = @@item
      i.each do |it|
        html '<div>'
        item = @@item += 1
        h = "GtkWidget* inspect_widget_#{item} = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);"
        h += "gtk_box_pack_start(GTK_BOX(inspect_widget_#{before}), inspect_widget_#{item}, 1, 1, 0);"
        c_code h
        box.pack_start(it.inspect(true))
        c_code ";{int loop_end=         #{@@item};   };"
        html '</div>'
      end
    end
    unless recurring
      add_c_footer
      add_html_footer
      window = Gtk::Window.new
      window.add(box)
      window.signal_connect('delete-event') { |_i| Gtk.main_quit }
      window.show_all
      #      Gtk.main
      return window
    end
    box
  end

  private

  def add_c_code_for_packing(i, arg)
    i = i.to_s
    #    to_function_name = i.gsub("

    function_name = i.split.join('_').downcase.chars.grep(/[a-z_]/).join
    new_list = %w[release].map do |word|
      "#{word}_#{function_name}"
    end

    $lists ||= []
    $lists.push new_list
    q = new_list.reverse
    dumped = i.dump
    a = "{GtkWidget* t = gtk_text_view_new();GtkTextBuffer* b = gtk_text_view_get_buffer(GTK_TEXT_VIEW(t));gtk_text_buffer_set_text(GTK_TEXT_BUFFER(      b), #{dumped}, #{i.length} /* strlen(#{dumped}) = #{i.length} */);"
    a += 'gtk_widget_add_events(t, GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK); '
    a += "c(t, \"button-release-event\", #{q.pop}, NULL);"
    #      "c(t, \"enter-notify-event\", #{q.pop},  NULL);",
    #     "c(t, \"leave-notify-event\", #{q.pop}, NULL);".join(' ')
    a += "gtk_box_pack_start(GTK_BOX(inspect_widget_#{arg}), t,1, 1, 0);  }"
    a += "\n"

    c_code a
  end
end

if ARGV.length.zero?
  puts 'Enter a list'.green
  exit 1
end

# system "rm *.exe"

eval(ARGV[0]).inspect

quit
