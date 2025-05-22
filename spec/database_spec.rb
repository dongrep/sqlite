describe 'database' do
  before do
    `rm -rf test_new.db`
  end

  def run_script(commands)
    raw_output = nil
    IO.popen(["./db", "test_new.db"], "r+") do |pipe|
      commands.each do |command|
      pipe.puts command
      end

      pipe.close_write

      # Read entire output
      raw_output = pipe.gets(nil)
    end
    raw_output.split("\n")
  end

  it 'inserts and retrieves a row' do
    result = run_script([
      "insert 1 user1 person1@example.com",
      "select",
      ".exit",
    ])
    expect(result).to match_array([
      "db > Executed.",
      "db > (1, user1, person1@example.com)",
      "Executed.",
      "db > ",
    ])
  end

  # it 'prints an error message when table is full' do
  #   script = []
  
  #   1401.times do |i|
  #     script << "insert #{i} user#{i} person#{i}@example.com"
  #   end
  
  #   script << ".exit"
  
  #   result = run_script(script)
  
  #   expect(result[-2]).to eq('Error: Table full.')
  # end

  it 'prints an error message when leaf node is full' do
    script = []
  
    14.times do |i|
      script << "insert #{i} user#{i} person#{i}@example.com"
    end
  
    script << ".exit"
  
    result = run_script(script)
  
    expect(result[-1]).to eq('db > Need to implement splitting a leaf node.')
  end



  it "allows to store maximum length strings" do
    long_username = "a"*32
    long_email = "a"*255
    script = [
      "insert 1 #{long_username} #{long_email}",
      "select",
      ".exit",
    ]
    result = run_script(script)
    expect(result).to match_array([
      "db > Executed.",
      "db > (1, #{long_username}, #{long_email})",
      "Executed.",
      "db > ",
    ])
  end

  it "blocks from storing longer than allowed strings" do
    long_username = "a"*34
    long_email = "a"*257

    script = [
      "insert 1 #{long_username} #{long_email}",
      "select",
      ".exit",
    ]
    result = run_script(script)
    expect(result).to match_array([
    "db > String is too long.",
    "db > Executed.",
    "db > ",
    ])
  end

  it "prints error if id is negative" do
    username = "a"
    email = "a@a.com"

    script = [
      "insert -1 #{username} #{email}",
      "select",
      ".exit",
    ]
    result = run_script(script)
    expect(result).to match_array([
    "db > ID must be positive.",
    "db > Executed.",
    "db > ",
    ])
  end

  it "keeps the data after closing connection" do 
    result1 = run_script([
      "insert 1 piyush piyush@example.com",
      ".exit"
    ])
    expect(result1).to match_array([
      "db > Executed.",
      "db > "
    ])
    result2 = run_script([
      "select",
      ".exit"
    ])
    expect(result2).to match_array([
      "db > (1, piyush, piyush@example.com)",
      "Executed.",
      "db > "
    ])
  end

  it 'prints constants' do
    script = [
      ".constants",
      ".exit",
    ]
    result = run_script(script)

    expect(result).to match_array([
      "db > Constants:",
      "ROW_SIZE: 293",
      "COMMON_NODE_HEADER_SIZE: 6",
      "LEAF_NODE_HEADER_SIZE: 10",
      "LEAF_NODE_CELL_SIZE: 297",
      "LEAF_NODE_SPACE_FOR_CELLS: 4086",
      "LEAF_NODE_MAX_CELLS: 13",
      "db > ",
    ])
  end

  it 'allows printing out the structure of a one-node btree' do
    script = [3,1,2].map do |i|
      "insert #{i} person#{i} person#{i}@example.com"
    end
    script << ".btree"
    script << ".exit"

    result = run_script(script)
    expect(result).to match_array([
      "db > Executed.",
      "db > Executed.",
      "db > Executed.",
      "db > Tree:",
      "leaf (size 3)",
      "  - 0 : 3",
      "  - 1 : 1",
      "  - 2 : 2",
      "db > "
    ])
  end
end
