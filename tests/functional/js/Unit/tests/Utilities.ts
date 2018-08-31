
export function WaitForCondition(condition: () => boolean, after: () => void) {
    if (condition() === true) {
        after();
    }
    else {
        setTimeout(() => WaitForCondition(condition, after), 500);
    }
}