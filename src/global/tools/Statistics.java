package global.tools;

public class Statistics {
    //Attributes
    private static Statistics uniqueInstance;
    private int states;
    private int transitions;
    private int events;
    private boolean decision;
    private int properties;
    private int propertiesCaseOne;
    private int propertiesCaseTwo;
    private int propertiesCaseThree;
    private int totalCounterexamples;
    private int validCounterexamples;
    private int invalidCounterexamples;

    //Constructor
    private Statistics () {
        reset();
    }

    //Methods
    public static Statistics getInstance() {
        if (uniqueInstance == null) {
            uniqueInstance = new Statistics();
        }
        return uniqueInstance;
    }

    public int getStates () {
        return states;
    }

    public void addState () {
        states ++;
    }

    public int getTransitions() {
        return transitions;
    }

    public void addTransition() {
        transitions ++;
    }

    public int getEvents() {
        return events;
    }

    public void addEvent() {
        events ++;
    }

    public boolean getDecision() {
        return decision;
    }

    public void setDecision() {
        decision = true;
    }

    public int getProperties() {
        return properties;
    }

    public void addProperty() {
        properties ++;
    }

    public int getPropertiesCaseOne() {
        return propertiesCaseOne;
    }

    public void addPropertyCaseOne() {
        propertiesCaseOne ++;
    }

    public int getPropertiesCaseTwo() {
        return propertiesCaseTwo;
    }

    public void addPropertyCaseTwo() {
        propertiesCaseTwo ++;
    }

    public int getPropertiesCaseThree() {
        return propertiesCaseThree;
    }

    public void addPropertyCaseThree() {
        propertiesCaseThree ++;
    }

    public int getTotalCounterexamples() {
        return totalCounterexamples;
    }

    public void setTotalCounterexamples(int totalCounterexamples) {
        this.totalCounterexamples = totalCounterexamples;
    }

    public int getValidCounterexamples() {
        return validCounterexamples;
    }

    public void setValidCounterexamples(int validCounterexamples) {
        this.validCounterexamples = validCounterexamples;
    }

    public int getInvalidCounterexamples() {
        return invalidCounterexamples;
    }

    public void setInvalidCounterexamples(int invalidCounterexamples) {
        this.invalidCounterexamples = invalidCounterexamples;
    }

    public void reset() {
        states = 0;
        transitions = 0;
        events = 0;
        decision = false;
        properties = 0;
        propertiesCaseOne = 0;
        propertiesCaseTwo = 0;
        propertiesCaseThree = 0;
        totalCounterexamples = 0;
        validCounterexamples = 0;
        invalidCounterexamples = 0;
    }
}
