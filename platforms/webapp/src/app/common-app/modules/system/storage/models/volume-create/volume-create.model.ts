import { VolumePolicyModel } from './volume-policy.model';
import { VolumeMembersModel } from './volume-members.model';

export class VolumeCreateModel {
    private constructor() {}

    set setButtonState(buttonState: boolean) {
        this.buttonState = buttonState;
    }

    get getButtonState() {
        return this.buttonState;
    }

    private static instance: VolumeCreateModel;

    private buttonState: boolean = true;

    public static getInstance(): VolumeCreateModel {
        if (!VolumeCreateModel.instance) {
            VolumeCreateModel.instance = new VolumeCreateModel();
        }
        return VolumeCreateModel.instance;
    }

    public static destroy(): void {
        if (VolumeCreateModel.instance) {
            VolumeCreateModel.instance = null;
        }
        VolumePolicyModel.destroy();
        VolumeMembersModel.destroy();
    }

    public checkBtnState() {
        const policyState = VolumePolicyModel.getInstance().checkSave();
        const memberState = VolumeMembersModel.getInstance().checkSave();

        if (policyState ||  memberState) {
            this.buttonState = true;
        } else {
            this.buttonState = false;
        }
    }

}
