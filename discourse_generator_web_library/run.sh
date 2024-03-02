(grep -P '^\s*(?:([$_a-zA-Z0-9]+ = )|(?:ifffffffffff \((.+)\)))' discourse_generator_online_widget2.js  | \
              t "i.strip.split.select {|i| i!='if'}.first.gsub(/[^_a-zA-Z0-9]/, '')    + '=false'" | \
              sort | uniq | ruby -e \
"m=STDIN.read.strip.split.sort{|m, k| m.length<=>k.length}.join(44.chr); warn m; puts '/* LICENSED UNDER 0BSD rubygems.org/gems/democracy */        const ask =      (function(' + m + '){ function debug() { console.log('+m.split(/=false,?/).map { |i| [i, i].join(':')    }.join(',').prepend('{')  + '}) } ' + 10.chr"      ; \
\
cat discourse_generator_online_widget2.js | squeeze 1 1;   echo  '})();')  | \
sponge discourse_generator_online_widget2.js

node --check script
#prettier discourse_generator_online_widget2.js -w||exit 1     # # || exit;


google-chrome index.html discourse_generator_online_widget2.js
