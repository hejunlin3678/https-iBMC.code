export class UniqueIdDisposer {
    public currentUniqueId = 15;

    getNextUniqueId() {
        this.currentUniqueId++;
        if ((this.currentUniqueId & 0xffffffff) === 0) {
            this.currentUniqueId |= 0x00000010;
        }
        return this.currentUniqueId;
    }
}
