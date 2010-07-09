"""
@brief The trivial classifier that just copies the FT1EventClass value.
"""
#
# $Header$
#
meritVariables = """
FT1EventClass
""".split()

eventClassifier = lambda row : row['FT1EventClass']
