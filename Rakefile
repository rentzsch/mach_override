desc 'Build'
task :build do
  system('mkdir build')
  system('gcc -o build/test_gcc_i386 -m32 -framework CoreServices *.c *.cp libudis86/*.c')
  system('gcc -o build/test_gcc_x86_64 -m64 -framework CoreServices *.c *.cp libudis86/*.c')
end

desc 'Test'
task :test do
  system('build/test_gcc_i386')
  system('build/test_gcc_x86_64')
end

desc 'Clean up'
task :clean do
  system('rm -rf build')
end

task :default => [:build, :test]