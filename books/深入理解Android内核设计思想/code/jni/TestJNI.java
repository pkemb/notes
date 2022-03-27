public class TestJNI {
    private native int testJniAdd(int a, int b);
    private native int testJniAdd2(int []arr);
    public void test(){
        System.out.println("The result = " + testJniAdd(2, 3));
    }
    public static void main(String[] args) {
        TestJNI JniExample = new TestJNI();
        JniExample.test();
    }
    static {
        System.loadLibrary("testJniLib");
    }
}