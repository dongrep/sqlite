for i in $(seq 1 2 24); do
  echo "insert $i user$i email$i@example.com"
  if [ "$i" -eq 23 ]; then
    echo ".exit"
  fi
done | ./db demo.db
