local inspect = require "inspect"

print(runtime)
if runtime.isDebug() then
  print("booting....")

  -- if settings then
  --   print("resolution:" .. inspect(settings.getResolution("resolution")))
  -- end
end