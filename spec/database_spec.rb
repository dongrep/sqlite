describe 'database' do
  before do
    `rm -rf test.db`
  end

  def run_script(commands)
    raw_output = nil
    IO.popen(["./db", "test.db"], "r+") do |pipe|
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

  it 'prints error message when table is full' do
    script = (1..1401).map do |i|
      "insert #{i} person#{i} person#{i}@exampl.com"
    end
    script << ".exit"
    result = run_script(script)
    expect(result[-2]).to eq('db > Error: Table full.')
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

end
