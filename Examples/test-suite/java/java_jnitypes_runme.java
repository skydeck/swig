
import java_jnitypes.*;

public class java_jnitypes_runme {

  static {
    try {
	System.loadLibrary("java_jnitypes");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
      System.exit(1);
    }
  }

  public static class Test {
  }

  public static void main(String argv[]) {

    Test test = new Test();
    boolean booleanArray[] = new boolean[] {true, false};
    char charArray[] = new char[] {10, 20};
    byte byteArray[] = new byte[] {'A', 'B'};
    short shortArray[] = new short[] {10, 20};
    int intArray[] = new int[] {10, 20};
    long longArray[] = new long[] {10, 20};
    float floatArray[] = new float[] {(float)10.0, (float)20.0};
    double doubleArray[] = new double[] {10.0, 20.0};
    Test objectArray[] = new Test[] {new Test(), test};

    if (java_jnitypes.jnifunc(true) != true)                testFailed("jboolean");
    if (java_jnitypes.jnifunc(100) != 100)                  testFailed("jchar");
    if (java_jnitypes.jnifunc('A') != 'A')                  testFailed("jbyte");
    if (java_jnitypes.jnifunc(100) != 100)                  testFailed("jshort");
    if (java_jnitypes.jnifunc(100) != 100)                  testFailed("jint");
    if (java_jnitypes.jnifunc(100) != 100)                  testFailed("jlong");
    if (java_jnitypes.jnifunc(100) != 100)                  testFailed("jfloat");
    if (java_jnitypes.jnifunc(100.0) != 100)                testFailed("jdouble");
    if (java_jnitypes.jnifunc("100") != "100")              testFailed("jstring");
    if (java_jnitypes.jnifunc(test) != test)                testFailed("jobject");
    if (java_jnitypes.jnifunc(booleanArray)[1] != false)    testFailed("jbooleanArray");
    if (java_jnitypes.jnifunc(charArray)[1] != 20)          testFailed("jcharArray");
    if (java_jnitypes.jnifunc(byteArray)[1] != 'B')         testFailed("jbyteArray");
    if (java_jnitypes.jnifunc(shortArray)[1] != 20)         testFailed("jshortArray");
    if (java_jnitypes.jnifunc(intArray)[1] != 20)           testFailed("jintArray");
    if (java_jnitypes.jnifunc(longArray)[1] != 20)          testFailed("jlongArray");
    if (java_jnitypes.jnifunc(floatArray)[1] != 20.0)       testFailed("jfloatArray");
    if (java_jnitypes.jnifunc(doubleArray)[1] != 20.0)      testFailed("jdoubleArray");
    if (java_jnitypes.jnifunc(objectArray)[1] != test)      testFailed("jobjectArray");

  }

  public static void testFailed(String str) {
      System.err.println(str + " test failed");
      System.exit(1);
  }
}
