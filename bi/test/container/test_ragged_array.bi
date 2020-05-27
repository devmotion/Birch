/*
 * Test RaggedArray.
 */
program test_ragged_array() {
  o:RaggedArray<Integer>;
  
  o.pushBack();
  o.pushBack(1, 1);
  o.pushBack(1, 2);
  o.pushBack(1, 3);
  o.pushBack();
  o.pushBack(2, 4);
  o.pushBack();
  o.pushBack(3, 5);
  if !check_ragged_array(o, [3, 1, 1], [1, 2, 3, 4, 5]) {
    exit(1);
  }
  
  o.pushBack(2, 6);
  if !check_ragged_array(o, [3, 2, 1], [1, 2, 3, 4, 6, 5]) {
    exit(1);
  }
}

function check_ragged_array(o:RaggedArray<Integer>, sizes:Integer[_],
    values:Integer[_]) -> Boolean {
  auto result <- true;
  
  /* number of rows */
  if o.size() != length(sizes) {
    stderr.print("incorrect total size\n");
    result <- false;
  }
  
  /* length of each row */
  for i in 1..length(sizes) {
    if o.size(i) != sizes[i] {
      stderr.print("incorrect row size\n");
      result <- false;
    }
  }
  
  /* contents */
  auto k <- 1;
  for i in 1..o.size() {
    for j in 1..o.size(i) {
      if o.get(i, j) != values[k] {
        stderr.print("incorrect value\n");
        result <- false;
      }
      k <- k + 1;
    }
  }

  return result;
}
