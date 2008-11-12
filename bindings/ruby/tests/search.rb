#
# Search
#

$:.unshift "../../../build/bindings/ruby"
require 'pathname'

# test Repodata
require 'test/unit'
require 'satsolver'

class RepodataTest < Test::Unit::TestCase
  #
  # Pool search
  #
  def test_pool_search
    pool = Satsolver::Pool.new
    assert pool
    pool.arch = "x86_64"
    solvpath = Pathname( File.dirname( __FILE__ ) ) + Pathname( "../../testdata" ) + "os11-biarch.solv"
    repo = pool.add_solv( solvpath )
    repo.name = "openSUSE 11.0 Beta3 BiArch"
    puts "Repo #{repo.name} loaded with #{repo.size} solvables"
    
    for d in pool.search("yast2", satsolver.SEARCH_STRING):
      print d.solvable(), "matches 'yast2' in ", d.key(), ":  ", d.value()
    assert True
    
  def test_pool_search_files(self):
    pool = satsolver.Pool()
    assert pool
    pool.set_arch("i686")
    solvpath = Pathname( File.dirname( __FILE__ ) ) + Pathname( "../../testdata" ) + "os11-biarch.solv"
    repo = pool.add_solv( solvpath )
    repo.set_name("test")
    for d in pool.search("/usr/bin/python", satsolver.SEARCH_STRING|satsolver.SEARCH_FILES):
      print d.solvable(), "matches '/usr/bin/python' in ", d.key(), ":  ", d.value()
    assert True

  #
  # Repo search
  #
  def test_repo_search
    pool = Satsolver::Pool.new
    assert pool
    pool.arch = "x86_64"
    solvpath = Pathname( File.dirname( __FILE__ ) ) + Pathname( "../../testdata" ) + "os11-biarch.solv"
    repo = pool.add_solv( solvpath )
    repo.name = "openSUSE 11.0 Beta3 BiArch"
    puts "Repo #{repo.name} loaded with #{repo.size} solvables"
    
    for d in repo.search("yast2", satsolver.SEARCH_STRING):
      print d.solvable(), "matches 'yast2' in ", d.key(), ":  ", d.value()
    assert True
    
  def test_repo_search_files(self):
    pool = satsolver.Pool()
    assert pool
    pool.set_arch("i686")
    solvpath = Pathname( File.dirname( __FILE__ ) ) + Pathname( "../../testdata" ) + "os11-biarch.solv"
    repo = pool.add_solv( solvpath )
    repo.set_name("test")
    for d in repo.search("/usr/bin/python", satsolver.SEARCH_STRING|satsolver.SEARCH_FILES):
      print d.solvable(), "matches '/usr/bin/python' in ", d.key(), ":  ", d.value()
    assert True

if __name__ == '__main__':
  unittest.main()
