package global.tools;

public class IdGenerator {
    //Attributes
    private static int integerId;

    //Constructor
    private IdGenerator () {
        integerId = 0;
    }

    //Static Methods
    public static void reset () {
        integerId = 0;
    }

    public static int getIntegerId () {
        return ++ integerId;
    }

}
