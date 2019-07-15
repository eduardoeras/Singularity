package global.tools;

public class IdGenerator {
    //Attributes
    private static IdGenerator uniqueInstance;
    private int integerId;

    //Constructor
    private IdGenerator () {
        integerId = 0;
    }

    //Static Methods
    public static IdGenerator getInstance () {
        if (uniqueInstance == null) {
            uniqueInstance = new IdGenerator();
        }
        return uniqueInstance;
    }

    //Public Methods
    public int getIntegerId () {
        return ++ integerId;
    }

    public void reset () {
        integerId = 0;
    }
}
