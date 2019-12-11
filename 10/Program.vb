' Compile:
'     dotnet publish -c Release --self-contained --runtime linux-x64
' Run:
'     ./bin/Release/netcoreapp3.1/linux-x64/publish/10 < input.txt
' Compiler version:
'     dotnet --version
'     3.1.100

' Since we're trying to do a different language every day, we also use some unpopular ones.
' VB.NET has gained meny features that C# recently got. It developed very well.

Imports System
Imports System.Linq
Imports System.Numerics
Imports System.Collections.Generic

module Day10

	function CountVisibleAsteroids(field as List(of List(of Boolean)), width as Integer, height as Integer, origin as Vector2) as Integer
		dim slopesWithAsteroids = new HashSet(of Single)

		for x as Integer = 0 to width - 1
			for y as Integer = 0 to height - 1
				dim isAstroid = field(y)(x)

				if isAstroid then
					dim here = new Vector2(x, y)
					if here = origin then continue for

					dim d = origin - here

					dim slope = Math.Atan2(d.X, d.Y)

					slopesWithAsteroids.Add(slope)
				end if
			next
		next

		return slopesWithAsteroids.Count
	end function

	function ReadInput() as List(of List(of Boolean))
		dim field = new List(of List(of Boolean))

		dim line as String
		do
			line = Console.ReadLine()
			if line isnot Nothing then
				dim lineData = line.ToCharArray().Select(function(c) c = "#").ToList()
				field.Add(lineData)
			end if
		loop until line is Nothing

		return field
	end function

	sub Main(args As String())

		dim field = ReadInput()

		dim width = field.Max(function(l) l.Count)
		dim height = field.Count

		Console.WriteLine($"Field Size: {width}x{height}")

		dim res = new Dictionary(of (x as Integer, y as Integer), Integer)

		for x as Integer = 0 to width - 1
			Console.Write(vbTab)
			for y as Integer = 0 to height - 1
				dim visibleAsteroids = CountVisibleAsteroids(field, width, height, new Vector2(x, y))
				res.Add((x, y), visibleAsteroids)

				Console.Write(visibleAsteroids.ToString().PadLeft(4))
			next
			Console.WriteLine()
		next

		dim max = res.Max(function (e) e.Value)

		Console.WriteLine()
		Console.WriteLine($"Maximum visible asteroids (Part 1 Answer): {max}")
	end sub
end module
