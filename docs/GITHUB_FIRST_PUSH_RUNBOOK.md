# GitHub first-push runbook

Status: **upstream written responses received; perform the final audit and
obtain explicit action-time confirmation before executing the first push.**

Repository: <https://github.com/favoritejonny/Emule-Next>

The local remote is already configured as:

`https://github.com/favoritejonny/Emule-Next.git`

Git for Windows 2.53.0 and Git Credential Manager 2.7.3 are available on the
project PC. GitHub CLI is not installed and is not required. Authentication
must use the browser opened by Git Credential Manager. Never paste a GitHub
password, two-factor code or access token into project files or chat.

## Preconditions

Do not start the first push until all of the following are true:

1. `LEGAL_STATUS.md` has no unresolved public-release blocker.
2. Any written permissions are retained with the private project records and
   their required wording is reflected in the public notices.
3. The working tree is clean and the source has passed the privacy, secret,
   binary-artifact and licence checks.
4. A current recoverable backup exists outside the working folder.
5. The public GitHub repository is still empty or its unexpected contents have
   been reviewed before any reconciliation.

## First source push

The maintainer and Codex should perform these steps together:

1. Recheck the configured remote and local branch.
2. Re-run repository integrity and secret scans.
3. Run the normal non-forced push of local `main` to `origin`.
4. Complete authentication only in the GitHub browser window opened by Git
   Credential Manager.
5. Verify that the browser identifies the expected account and repository
   before approving access.
6. Never use a forced push for the initial publication.
7. After completion, open the repository while signed out and verify the
   README, licence, notices, security policy, issue forms and source tree.

The eventual Git operation is:

`git push -u origin main`

It must not be run before the preconditions are satisfied.

## Protect `main` after the first push

After `main` exists online, open **Settings**, then **Rules** and create a
branch ruleset targeting the default branch. At minimum, block force pushes
and branch deletion. Do not enable a rule that the sole maintainer cannot
satisfy. Pull-request requirements can be added after another trusted reviewer
joins the project.

Keep private vulnerability reporting enabled and confirm that the Security
page offers the private report button.

## Release tag and binaries

Do not create `v1.0.0-alpha.1` during the first source push. First resolve the
release blockers, freeze the source, rebuild Win32 and x64 from the approved
commit, regenerate portable ZIPs and checksums, and complete the release
checklist. Then create the tag on that exact commit and treat it as immutable.

If an unexpected file, identity, commit or online change appears at any point,
stop. Do not delete history or force-push while investigating.
