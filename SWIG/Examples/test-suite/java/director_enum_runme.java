
import director_enum.*;

public class director_enum_runme {

  static {
    try {
      System.loadLibrary("director_enum");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
      System.exit(1);
    }
  }

  public static void main(String argv[]) {

      MyFoo a = new MyFoo();

      if (a.ping(Hallo.awright) != Hallo.yo) {
          throw new RuntimeException ( "a.ping()" );
      }

      Foo b = new Foo();

      if (b.ping(Hallo.awright) != Hallo.awright) {
          throw new RuntimeException ( "b.ping()" );
      }
  }
}

class MyFoo extends Foo {
    public Hallo say_hi(Hallo h) {
        return Hallo.yo;
    }
}

