
-- We provide the function power and we reuse it in the c function power_test, 
-- which we set in the lua interpreter

function power (x, y)
  local sum = 1 
  for i = 1, y do
    sum = sum * x
  end
  return sum
end

power_test(2, 4)
crystals_test("Crystals is the most awesome game ever.")
