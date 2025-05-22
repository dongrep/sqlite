for i in $(seq 1 1401); do
  echo "insert $i user$i email$i@example.com"
done | ./db demo.db
