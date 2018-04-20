local newcoroutine = coroutine.create
local await = coroutine.resume
local yield = coroutine.yield

local function async(func)
  return newcoroutine(func)
end

function counter(num)
  for i = 0, num do
    coroutine.yield(i)
  end
end

local a = coroutine.create(counter)

print(a)
print(coroutine.resume(a, 20))

local function abc()
  return print(function(...) end)
end

print(abc())
print(abc())