#!/usr/bin/env ruby
#
# runtest.rb
#
# ruby script to run zypp/libredcarpet testcases
#
# Usage:
#  runtest.rb <testcase1> [... <testcaseN>]
#     run one or more test cases
#  runtest.rb [-r] <dir>
#     run all test cases (*test.xml) below a specific directory
#     if -r is given, recursively descend to sub-directories
#

require 'test/unit'
require 'pp'

$verbose = false
$redcarpet = false

$tests = Array.new
$deptestomatic = File.join( Dir.getwd, "deptestomatic" )

class Solution
  # poor mans diff
  def Solution.filediff name1, name2
    begin
      f1 = File.new( name1, "r" )
    rescue
      STDERR.puts "Cannot open #{name1}"
      return false
    end
    begin
      f2 = File.new( name2, "r" )
    rescue
      STDERR.puts "Cannot open #{name2}"
      return false
    end
    a1 = f1.readlines
    a2 = f2.readlines
    i = 0
    a1.each { |l1|
      if (l1 =~ /unflag/)
	next
      end
      l2 = a2[i]
      if (l2 =~ /unflag/)
	i += 1
	retry
      end
      if l2
	if (l1 != l2)
	  puts "- #{l1}"
	  puts "+ #{l2}"
	end
      else
#	puts "- #{l1}"
      end
      i += 1
    }
    while i < a2.size
      puts "+ #{a2[i]}"
      i += 1
    end
  end

  def Solution.read fname
    solutions = Array.new
    solution = nil
    
    # read solution and filter irrelevant lines
    IO.foreach( fname ) { |l|
      case l.chomp!
      when /Installing/, /unflag/
	next
      when /Solution \#([0-9]+):/
	solution = Array.new
      when /installs=/
	solutions << solution.sort! unless solution.empty?
	solution = nil
      when /> install /, /> upgrade /, /> remove /
	STDERR.puts "No 'Solution' in #{fname}" unless solution
	solution << l
      end
    }
    
    solutions
  end

  # compare solution s with result r
  def Solution.compare sname, rname
    unless File.readable?( sname )
      STDERR.puts "Cannot open #{sname}"
      return false
    end
    unless File.readable?( rname )
      STDERR.puts "Cannot open #{rname}"
      return false
    end
    
    solutions = Solution.read sname
    results = Solution.read rname
    
    return true if (solutions.empty? && results.empty?)
    r = results.first
    solutions.each { |s|
      return true if s == r
    }
#    STDERR.puts "#{rname} failed"
#    STDERR.puts "Solution:"
#    pp solutions.first
#    STDERR.puts "Result:"
#    pp r
    return false
  end

end


class Tester < Test::Unit::TestCase
  
  def test_run
    passed = 0
    puts "#{$tests.size} tests ahead"
    $tests.sort!
    $tests.each { |test|
#      puts "Testing #{test}"
      basename = File.basename(test, ".xml")
      print "."
      STDOUT.flush
      dir = File.dirname(test)
      args = ""
      args = "--redcarpet" if $redcarpet
      if ( system( "(cd #{dir}; #{$deptestomatic} #{args} #{basename}.xml > #{basename}.result)" ) )
        sname = File.join( dir, "#{basename}.solution" )
        rname = File.join( dir, "#{basename}.result" )
	result = Solution.compare( sname, rname ) 
	passed += 1 if result
#        assert(  )
#      puts "(cd #{File.dirname(test)}; #{$deptestomatic} #{basename}.xml > #{basename}.result)" 
      end
    }
    failed = $tests.size - passed
    puts "\n\t==> #{$tests.size} tests: #{passed} passed, #{failed} failed <==\n"
  end
end

class Runner
 
  def run wd, arg, recurse=nil
    fullname = File.join( wd, arg )
    if File.directory?( fullname )
      rundir( fullname, recurse ) 
    elsif (arg =~ /test.xml$/)
#      puts "Run #{fullname}"
      $tests << fullname
    end
  end
  
  # process current directory
  #
  def rundir path, recurse
#    puts "Rundir #{path}"
    dir = Dir.new( path )

    dir.each { |name|
      if File.directory?( name )
	rundir File.join( path, name ), recurse if recurse
      else
	run path, name
      end
    }
    
  end
  
end

#----------------------------

def usage err=nil
  STDERR.puts "** Error: #{err}" if err
  STDERR.puts "Usage:"
  STDERR.puts "\truntest.rb <testcase1> [... <testcaseN>]"
  STDERR.puts "\t\trun one or more test cases"
  STDERR.puts "\truntest.rb [-r] <dir>"
  STDERR.puts "\t\trun all test cases (*test.xml) below a specific directory"
  STDERR.puts "\t\tif -r is given, recursively descend to sub-directories"
  exit 1
end

#------
# main

puts "Running in #{Dir.getwd}"

if ARGV.first == "--redcarpet"
  $redcarpet = true
  ARGV.shift
end

if ARGV.first == "-r"
  recurse = true
  ARGV.shift
end

if ARGV.first == "-v"
  $verbose = true
  ARGV.shift
end

r = Runner.new

ARGV.each { |arg|
  wd = Dir.getwd unless arg[0,1] == "/"
  r.run wd, arg, recurse
}