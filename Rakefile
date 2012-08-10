desc 'Build'
task :build do
  system('mkdir build')
  system('gcc -o build/test_gcc_i386 -framework CoreServices *.c *.cp')
end

desc 'Test'
task :test do
  system('build/test_gcc_i386')
end

desc 'Clean up'
task :clean do
  system('rm -rf build')
end

task :default => [:build, :test]