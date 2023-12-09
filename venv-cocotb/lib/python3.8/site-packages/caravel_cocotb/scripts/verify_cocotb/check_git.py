import subprocess
import os


class GitRepoChecker:
    def __init__(self, repo_path):
        self.repo_path = repo_path
        self.check_status()

    def check_status(self):
        # Store the current working directory
        original_dir = os.getcwd()
        repo_name = self._get_repo_name()
        try:
            # Change to the repository directory
            os.chdir(self.repo_path)

            # Run git fetch command to update remote tracking branches
            subprocess.call(['git', 'fetch'])
            try:
            # Get the local branch name
                branch_output = subprocess.check_output(['git', 'symbolic-ref', '--short', 'HEAD']).decode('utf-8')
                branch_name = branch_output.strip()
            except:
                raise RepoNotSyncedException(f"{TerminalColors.colorize(repo_name, TerminalColors.RED)} isn't synced any a known branch. please fix this. use -no_pull flag if you want to ignore this error.")

            # Compare local and remote commit hashes
            local_hash = self._get_commit_hash('HEAD')
            remote_hash = self._get_commit_hash(f'origin/{branch_name}')

            if local_hash == remote_hash:
                print(TerminalColors.colorize(f"{repo_name} is up to date.", TerminalColors.GREEN))
            else:
                # Prompt the user to pull the latest changes
                raise RepoNotSyncedException(f"{TerminalColors.colorize(repo_name, TerminalColors.RED)} is synced to commit {local_hash} while remote is at commit {remote_hash}. please pull the latest changes for {TerminalColors.colorize(self.repo_path, TerminalColors.BLUE)} or use -no_pull flag if you want to ignore this error.")
        finally:
            # Change back to the original directory
            os.chdir(original_dir)

    def _get_commit_hash(self, ref):
        # Get the commit hash for a given reference
        commit_hash = subprocess.check_output(['git', 'rev-parse', ref]).decode('utf-8').strip()
        return commit_hash

    def _get_repo_name(self):
        url = "https://github.com/" + f"{subprocess.run(f'cd {self.repo_path};git ls-remote --get-url', stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, shell=True).stdout}".replace("git@github.com:", "").replace(".git", "")   
        repo_name = f"Repo: {subprocess.run(f'cd {self.repo_path};basename -s .git `git config --get remote.origin.url`', stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, shell=True).stdout} ({url})".replace(".git", "").replace("https://github.com/", "").replace("\n", "").replace("Repo: ", "")
        return repo_name


class RepoNotSyncedException(Exception):
    def __init__(self, message):
        self.message = message
        super().__init__(self.message)


class TerminalColors:
    """
    Utility class for colorizing text in the terminal.
    """
    # ANSI escape codes for various colors
    RED = '\033[91m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    WHITE = '\033[97m'
    RESET = '\033[0m'


    @classmethod
    def colorize(cls, text, color):
        """
        Colorize the given text with the specified color.

        :param text: The text to be colorized.
        :param color: The color to apply.
        :return: The colorized text.
        """
        return f"{color}{text}{cls.RESET}"