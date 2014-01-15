# -*- ruby -*-

# To release this ruby gem type:
#  rake release VERSION=x.y.z
# where x.y.z is the appropriate version number of this gem.

require 'rubygems'
require 'hoe'

# Hoe.plugin :compiler
# Hoe.plugin :gem_prelude_sucks
# Hoe.plugin :inline
# Hoe.plugin :minitest
# Hoe.plugin :racc
# Hoe.plugin :rubyforge

# generate the Manifest.txt file (before we invoke Hoe.spec)
manifest = FileList[
  '.gitignore', 
  'History.*', 
  'README.*', 
  'Rakefile',
  'bin/**/*',
  'lib/**/*.rb',
  'test/**/*.rb',
];
File.open('Manifest.txt', 'w') do | manifestFile |
  manifestFile.puts("Manifest.txt");
  manifestFile.write(manifest.to_a.join("\n"));
end

# For dependency documentation see:
#   http://guides.rubygems.org/patterns/

Hoe.spec 'cook' do
  developer('Stephen Gaito', 'stephen@perceptisys.co.uk')
  dependency('rake', '~> 0.9.2');
  dependency('construct', '~> 0.1.7');
  dependency('erubis', '~> 2.7');
  dependency('greenletters', '~> 0.2');
  dependency('gpgme', '~> 2.0');
  dependency('highline', '~> 1.6.1');
end

# vim: syntax=ruby
