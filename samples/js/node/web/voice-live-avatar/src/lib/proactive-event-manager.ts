// The ProactiveEventManager will do greeting when the app starts and 
//  trigger the onNoActivity function when there is no activity for a certain period of time.
export class ProactiveEventManager {
    public onGreeting:  () => Promise<void> = async () => {
        // Default greeting handler
    }

    public onNoActivity: () => Promise<void> = async () => {
        // Default no activity handler
    }

    private lastActivityTime: Date;
    private timer!: NodeJS.Timeout;
    private interval: number;

    constructor(onGreeting: () => Promise<void>, onInactive: () => Promise<void>, interval: number = 60000) {
        this.lastActivityTime = new Date(); // Initialize with current time
        this.onGreeting = onGreeting;
        this.onNoActivity = onInactive;
        this.interval = interval; // Set the interval for inactivity check
    }

    /**
     * Updates the last activity time to the current time
     */
    public updateActivity(activityName: string =  ""): void { // eslint-disable-line
        this.lastActivityTime = new Date();
    }

    /**
     * Starts the inactivity timer that checks for inactivity every minute
     */
    public start(): void {
        this.onGreeting();
        this.stopTimer(); // Ensure no existing timer is running
    
        // Start a new timer that will begin checking after the first interval
        this.timer = setTimeout(() => {
            this.startRecurringCheck();
        }, this.interval);
    }

        /**
     * Starts the recurring interval check for inactivity
     */
    private startRecurringCheck(): void {
        // Clear any existing timer first to prevent duplicates
        this.stopTimer();
        
        // Set up the recurring interval
        this.timer = setInterval(async () => {
            await this.checkInactivity();
        }, this.interval);
    }

    /**
     * Stops any running timer
     */
    private stopTimer(): void {
        if (this.timer) {
            clearTimeout(this.timer);
            clearInterval(this.timer);
            this.timer = null as unknown as NodeJS.Timeout; // Reset the timer to null
        }
    }

    /**
     * Checks if the last activity was over 1 minute ago
     */
    private async checkInactivity(): Promise<void> {
        const now = new Date();
        const timeDifference = now.getTime() - this.lastActivityTime.getTime();

        if (timeDifference > this.interval) {
            await this.onNoActivity();
            this.lastActivityTime = now; // Reset the last activity time
        }
    }

    /**
     * Cleans up the timer to prevent memory leaks
     */
    public stop(): void {
        if (this.timer) {
            clearInterval(this.timer);
        }
    }
}